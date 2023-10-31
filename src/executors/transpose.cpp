#include "global.h"
/**
 * @brief
 * SYNTAX: TRANSPOSE MATRIX matrix_name
 */
bool syntacticParseTRANSPOSE()
{
    logger.log("syntacticParseTRANSPOSE");
    if (tokenizedQuery.size() != 3 or tokenizedQuery[1] != "MATRIX")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = TRANSPOSE;
    parsedQuery.transposeRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseTRANSPOSE()
{
    logger.log("semanticParseTRANSPOSE");

    if (!tableCatalogue.isTable(parsedQuery.transposeRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeTRANSPOSE()
{
    logger.log("executeTRANSPOSE");
    Table *table = tableCatalogue.getTable(parsedQuery.transposeRelationName);
    table->readAccess = table->writeAccess = 0;
    table->setReadWriteZero();
    table->transpose();
    table->getBlockStatistics();
    return;
}
