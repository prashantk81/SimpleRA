#include "tableCatalogue.h"

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    ORDER_BY,
    PRINT,
    PRINT_MATRIX,
    PROJECTION,
    RENAME,
    RENAME_MATRIX,
    SELECTION,
    SORT,
    SOURCE,
    LOAD_MATRIX,
    TRANSPOSE,
    CHECKSYMMETRY,
    COMPUTE,
    EXPORT_MATRIX,
    GROUP,
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";
    string computeRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";
    string exportMatrixRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";

    string loadRelationName = "";

    string orderByResultRelationName = "";
    string orderByRelationName = "";
    string orderByAttr = "";
    string orderByStrategy = "";

    string printRelationName = "";
    string printMatrixName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList = {};
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";
    string renameMatrixFromName = "";
    string renameMatrixToName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    string sortColumnName = "";
    string sortRelationName = "";
    vector<string> sortAttributes = {};
    vector<string> sortAttributesStrategy = {};

    string sourceFileName = "";
    string symmetryRelationName = "";
    string transposeRelationName = "";

    // groupBY
    BinaryOperator groupByBinaryOperator = NO_BINOP_CLAUSE;
    string groupByResultRelationName = "";
    string groupByGroupAttributeName = "";
    string groupByRelationName = "";
    string groupByHavingAggregateFunctionName = "";
    string groupByHavingAttributeName = "";
    string groupByHavingConditionValue = "";
    string groupByReturnaggregateFunctionName = "";
    string groupByReturnAttributeName = "";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParsePRINT();
bool syntacticParsePRINTMATRIX();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseRENAMEMATRIX();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();
bool syntacticParseTRANSPOSE();
bool syntacticParseCHECKSYMMETRY();
bool syntacticParseCOMPUTE();
bool syntacticParseOrderBy();

bool syntacticParseGROUP();
bool isFileExists(string tableName);
bool isQueryFile(string fileName);
