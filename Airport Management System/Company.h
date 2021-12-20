#ifndef __COMP__
#define __COMP__
#define DETAIL_PRINT
#ifdef  DETAIL_PRINT
#define  DETAIL_PRINT 1
#else 
#define  DETAIL_PRINT 0
#endif
#define SAVE_N_LOAD_BIN_FILE
#ifdef SAVE_N_LOAD_BIN_FILE
#define  SAVE_N_LOAD_BIN_FILE 1
#else 
#define  SAVE_N_LOAD_BIN_FILE 0
#endif

#include "Flight.h"
#include "AirportManager.h"
#include "GeneralList.h"

typedef enum { eNone, eHour, eDate, eSorceCode, eDestCode, eNofSortOpt } eSortOption;
static const char* sortOptStr[eNofSortOpt];
typedef unsigned char BYTE;

typedef struct
{
	char* name;
	int			flightCount;
	Flight** flightArr;
	eSortOption	sortOpt;
	LIST		flighDateList;
}Company;

int		initCompanyFromFile(Company* pComp, AirportManager* pManaer, const char* fileName);
void	initCompany(Company* pComp, AirportManager* pManaer);
void	initDateList(Company* pComp);
int		isUniqueDate(const Company* pComp, int index);
int		addFlight(Company* pComp, const AirportManager* pManager);

void	printCompany(const Company* pComp, char* name, ...);
void	printFlightsCount(const Company* pComp);
int		saveCompanyToFileByBytes(const Company* pComp, const char* fileName);
int		saveCompanyToFile(const Company* pComp, const char* fileName);
int		loadCompanyFromFile(Company* pComp, const AirportManager* pManager, const char* fileName);
int		loadCompanyFromFileByBytes(Company* pComp, const char* fileName);
void loadDateFromFileByBytes(Flight* flight, int flightDate);
int		saveCompanyToFileByBytes(const Company* pComp, const char* fileName);
void	sortFlight(Company* pComp);
void	findFlight(const Company* pComp);
eSortOption showSortMenu();

void	freeCompany(Company* pComp);

#endif

