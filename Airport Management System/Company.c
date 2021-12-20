#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "Company.h"
#include "Airport.h"
#include "General.h"
#include "fileHelper.h"

static const char* sortOptStr[eNofSortOpt] = {
	"None","Hour", "Date", "Airport takeoff code", "Airport landing code" };


int	initCompanyFromFile(Company* pComp, AirportManager* pManaer, const char* fileName)
{
	int chk;
	L_init(&pComp->flighDateList);
	if (SAVE_N_LOAD_BIN_FILE)
		chk = loadCompanyFromFileByBytes(pComp, fileName);
	else
		chk = (loadCompanyFromFile(pComp, pManaer, fileName));
	if (chk)
	{
		initDateList(pComp);
		return 1;
	}
	return 0;
}

void	initCompany(Company* pComp, AirportManager* pManaer)
{
	printf("-----------  Init Airline Company\n");
	L_init(&pComp->flighDateList);

	pComp->name = getStrExactName("Enter company name");
	pComp->flightArr = NULL;
	pComp->flightCount = 0;
}

void	initDateList(Company* pComp)
{
	for (int i = 0; i < pComp->flightCount; i++)
	{
		if (isUniqueDate(pComp, i))
		{
			char* sDate = createDateString(&pComp->flightArr[i]->date);
			L_insert(&(pComp->flighDateList.head), sDate);
		}
	}
}

int		isUniqueDate(const Company* pComp, int index)
{
	Date* pCheck = &pComp->flightArr[index]->date;
	for (int i = 0; i < pComp->flightCount; i++)
	{
		if (i == index)
			continue;
		if (equalDate(&pComp->flightArr[i]->date, pCheck))
			return 0;
	}
	return 1;
}

int		addFlight(Company* pComp, const AirportManager* pManager)
{

	if (pManager->count < 2)
	{
		printf("There are not enoght airport to set a flight\n");
		return 0;
	}
	pComp->flightArr = (Flight**)realloc(pComp->flightArr, (pComp->flightCount + 1) * sizeof(Flight*));
	CHECK_MSG_RETURN_0(pComp->flightArr, "Allocation error");
	pComp->flightArr[pComp->flightCount] = (Flight*)calloc(1, sizeof(Flight));
	CHECK_MSG_RETURN_0(pComp->flightArr[pComp->flightCount], "Allocation error");
	initFlight(pComp->flightArr[pComp->flightCount], pManager);
	if (isUniqueDate(pComp, pComp->flightCount))
	{
		char* sDate = createDateString(&pComp->flightArr[pComp->flightCount]->date);
		L_insert(&(pComp->flighDateList.head), sDate);
	}
	pComp->flightCount++;
	return 1;
}
void printCompany(const Company* pComp, char* name, ...)
{
	char* StringSum = NULL;
	char* currentWord;
	va_list companyNames;
	va_start(companyNames, name);
	currentWord = name;
	printf("The Company: %s", pComp->name);
	while (currentWord != NULL)
	{
		printf("_%s", currentWord);
		currentWord = va_arg(companyNames, char*);
	}
	printf("\n");
	printf("Has %d flights\n", pComp->flightCount);
	printf("Sort: %s\n", sortOptStr[pComp->sortOpt]);
	if (DETAIL_PRINT)
	{
		generalArrayFunction((void*)pComp->flightArr, pComp->flightCount, sizeof(Flight**), printFlightV);
		printf("\nFlight Date List:");
		L_print(&pComp->flighDateList, printStr);
	}
}

void	printFlightsCount(const Company* pComp)
{
	char codeOrigin[CODE_LENGTH + 1];
	char codeDestination[CODE_LENGTH + 1];

	if (pComp->flightCount == 0)
	{
		printf("No flight to search\n");
		return;
	}

	printf("Origin Airport\n");
	getAirportCode(codeOrigin);
	printf("Destination Airport\n");
	getAirportCode(codeDestination);

	int count = countFlightsInRoute(pComp->flightArr, pComp->flightCount, codeOrigin, codeDestination);
	if (count != 0)
		printf("There are %d flights ", count);
	else
		printf("There are No flights ");

	printf("from %s to %s\n", codeOrigin, codeDestination);
}
//Company writing to Binary File
int		saveCompanyToFileByBytes(const Company* pComp, const char* fileName)
{
	//File opening
	FILE* btFile;
	btFile = fopen(fileName, "wb");
	CHECK_NULL__MSG_CLOSE_FILE(btFile, btFile, "Error open copmpany file to write");
	unsigned int companyNameLength = strlen(pComp->name);
	BYTE data[2];
	//Bit 0 flight count
	data[1] = pComp->flightCount >> 1;
	//Company name Length
	data[0] = companyNameLength;
	//Sort option
	data[0] = data[0] | ((pComp->sortOpt) << 4);
	//Bit 1-8 flight Count
	data[0] = (pComp->flightCount << 7) | data[0];
	//Writing first 2 bytes in block
	fwrite(data, sizeof(BYTE), 2, btFile);
	//Name Writing
	writeCharsToFile(pComp->name, companyNameLength + 1, btFile, "Error write company name\n");
	unsigned int flightDate = 0;
	for (int i = 0; i < pComp->flightCount; i++)
	{
		//IATA Source writing
		writeCharsToFile(pComp->flightArr[i]->originCode, CODE_LENGTH, btFile, "Error write IATA source\n");
		//IATA Destination writing
		writeCharsToFile(pComp->flightArr[i]->destCode, CODE_LENGTH, btFile, "Error write IATA destination\n");
		//year
		flightDate = (pComp->flightArr[i]->date.year) << 14;
		//Month
		flightDate = (flightDate) | (pComp->flightArr[i]->date.month) << 10;
		//Day
		flightDate = (flightDate) | (pComp->flightArr[i]->date.day) << 5;
		//Hour
		flightDate = (flightDate) | (pComp->flightArr[i]->hour);
		//writing the entire int as bytes.
		writeIntToFile(flightDate, btFile, "Error write int\n");
		flightDate = 0;
	}
	//File closing
	fclose(btFile);
	return 1;
}
void loadDateFromFileByBytes(Flight* flight, int flightDate)
{
	//Hour
	unsigned int hour = flightDate & 0x1F;
	flight->hour = hour;
	//Dat
	unsigned int day = (flightDate >> 5) & 0x1F;
	flight->date.day = day;
	//Month
	unsigned int month = (flightDate >> 10) & 0xF;
	flight->date.month = month;
	//Year
	unsigned int year = (flightDate >> 14) & 0x3FFFF;
	flight->date.year = year;
	return 1;
}
int loadCompanyFromFileByBytes(Company* pComp, const char* fileName)
{
	//File opening
	FILE* btFile = fopen(fileName, "rb");
	CHECK_NULL__MSG_CLOSE_FILE(btFile, btFile," Error open company file");
	BYTE data[2];
	unsigned int flightDate = 0;
	//Reading first two bytes
	fread(data, sizeof(BYTE), 2, btFile);
	//Flight count
	pComp->flightCount = (unsigned int)((data[1] << 1) | data[0] >> 7);
	//Sort option
	pComp->sortOpt = (unsigned int)((data[0] >> 4) & 7);
	//Airline name length
	unsigned int companyNameLen = (unsigned int)(data[0] & 0xF);
	pComp->name = (char*)malloc((companyNameLen + 1) * sizeof(char));
	CHECK_NULL__MSG_CLOSE_FILE(pComp->name, btFile, "Alocation error");
	//Airline name
	readCharsFromFile(pComp->name, (companyNameLen + 1), btFile, "Error reading company name\n");
	pComp->flightArr = (Flight**)malloc(pComp->flightCount * sizeof(Flight*));
	CHECK_NULL__MSG_CLOSE_FILE(pComp->flightArr, btFile, "Alocation error");
	for (int i = 0; i < pComp->flightCount; i++)
	{
		pComp->flightArr[i] = (Flight*)calloc(1, sizeof(Flight));
		CHECK_NULL__MSG_CLOSE_FILE(pComp->flightArr[i], btFile, "Alocation error");
		//IATA Source code
		if (!(readCharsFromFile(pComp->flightArr[i]->originCode, CODE_LENGTH, btFile, "Error reading IATA origin\n")))
			return 0;
		pComp->flightArr[i]->originCode[CODE_LENGTH] = '\0';
		//IATA Destination code
		if (!(readCharsFromFile(pComp->flightArr[i]->destCode, CODE_LENGTH, btFile, "Error reading IATA destination\n")))
			return 0;
		pComp->flightArr[i]->destCode[CODE_LENGTH] = '\0';
		//Date & flight hour int reader
		if (!(readIntFromFile(&flightDate, btFile, "Error reading file\n")))
			return 0;
		//Sending data & hour bytes to method to figure it out.
		loadDateFromFileByBytes(pComp->flightArr[i], flightDate);
	}
	//File closing
	fclose(btFile);
	return 1;
}


int		saveCompanyToFile(const Company* pComp, const char* fileName)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	CHECK_NULL__MSG_CLOSE_FILE(fp, fp, "Error open copmpany file to write");
	if (!writeStringToFile(pComp->name, fp, "Error write comapny name\n"))
		return 0;

	if (!writeIntToFile(pComp->flightCount, fp, "Error write flight count\n"))
		return 0;

	if (!writeIntToFile((int)pComp->sortOpt, fp, "Error write sort option\n"))
		return 0;

	for (int i = 0; i < pComp->flightCount; i++)
	{
		if (!saveFlightToFile(pComp->flightArr[i], fp))
			return 0;
	}

	fclose(fp);
	return 1;
}

int loadCompanyFromFile(Company* pComp, const AirportManager* pManager, const char* fileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	CHECK_NULL__MSG_CLOSE_FILE(fp, fp, "Error open company file");
	pComp->flightArr = NULL;
	pComp->name = readStringFromFile(fp, "Error reading company name\n");
	CHECK_RETURN_0(pComp->name);
	if (!readIntFromFile(&pComp->flightCount, fp, "Error reading flight count name\n"))
		return 0;

	int opt;
	if (!readIntFromFile(&opt, fp, "Error reading sort option\n"))
		return 0;

	pComp->sortOpt = (eSortOption)opt;

	if (pComp->flightCount > 0)
	{
		pComp->flightArr = (Flight**)malloc(pComp->flightCount * sizeof(Flight*));
		CHECK_NULL__MSG_CLOSE_FILE(pComp->flightArr, fp, "Alocation error");
	}
	else
		pComp->flightArr = NULL;
	for (int i = 0; i < pComp->flightCount; i++)
	{
		pComp->flightArr[i] = (Flight*)calloc(1, sizeof(Flight));
		CHECK_NULL__MSG_CLOSE_FILE(pComp->flightArr[i], fp, "Alocation error");
		if (!loadFlightFromFile(pComp->flightArr[i], pManager, fp))
			return 0;
	}
	fclose(fp);
	return 1;
}

void	sortFlight(Company* pComp)
{
	pComp->sortOpt = showSortMenu();
	int(*compare)(const void* air1, const void* air2) = NULL;

	switch (pComp->sortOpt)
	{
	case eHour:
		compare = compareByHour;
		break;
	case eDate:
		compare = compareByDate;
		break;
	case eSorceCode:
		compare = compareByCodeOrig;
		break;
	case eDestCode:
		compare = compareByCodeDest;
		break;

	}

	if (compare != NULL)
		qsort(pComp->flightArr, pComp->flightCount, sizeof(Flight*), compare);

}

void	findFlight(const Company* pComp)
{
	int(*compare)(const void* air1, const void* air2) = NULL;
	Flight f = { 0 };
	Flight* pFlight = &f;


	switch (pComp->sortOpt)
	{
	case eHour:
		f.hour = getFlightHour();
		compare = compareByHour;
		break;
	case eDate:
		getchar();
		getCorrectDate(&f.date);
		compare = compareByDate;
		break;
	case eSorceCode:
		getchar();
		getAirportCode(f.originCode);
		compare = compareByCodeOrig;
		break;
	case eDestCode:
		getchar();
		getAirportCode(f.destCode);
		compare = compareByCodeDest;
		break;
	}

	if (compare != NULL)
	{
		Flight** pF = bsearch(&pFlight, pComp->flightArr, pComp->flightCount, sizeof(Flight*), compare);
		if (pF == NULL)
			printf("Flight was not found\n");
		else {
			printf("Flight found, ");
			printFlight(*pF);
		}
	}
	else {
		printf("The search cannot be performed, array not sorted\n");
	}

}

eSortOption showSortMenu()
{
	int opt;
	printf("Base on what field do you want to sort?\n");
	do {
		for (int i = 1; i < eNofSortOpt; i++)
			printf("Enter %d for %s\n", i, sortOptStr[i]);
		scanf("%d", &opt);
	} while (opt < 0 || opt >eNofSortOpt);

	return (eSortOption)opt;
}

void	freeCompany(Company* pComp)
{
	generalArrayFunction((void*)pComp->flightArr, pComp->flightCount, sizeof(Flight**), freeFlight);
	free(pComp->flightArr);
	free(pComp->name);
	L_free(&pComp->flighDateList, freePtr);
}
