#pragma once

typedef enum
{
	eAddFlight, eAddAirport, ePrintCompany, ePrintAirports, ePrintFlightOrigDest,
	eSortFlights, eSearchFlight,
	eNofOptions
} eMenuOptions;

const char* str[eNofOptions];

#define EXIT			-1
#define MANAGER_FILE_NAME "airport_authority.txt"
#define COMPANY_FILE_NAME "company.bin"
#define COMPANY_FILE_NAME_IN_BYTES "companyWithBytes.bin"


int menu();
int initManagerAndCompany(AirportManager* pManager, Company* pCompany, char* cName, char* cByteName, char* mName);