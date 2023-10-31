#include "global.h"
/**
 * @brief
 * SYNTAX: COMPUTE matrix_name
 */
bool syntacticParseCOMPUTE()
{
    logger.log("syntacticParseCOMPUTE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTE;
    parsedQuery.computeRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParseCOMPUTE()
{
    logger.log("semanticParseCOMPUTE");

    if (!tableCatalogue.isTable(parsedQuery.computeRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeCOMPUTE()
{
    logger.log("executeCOMPUTE");
    Table *table = tableCatalogue.getTable(parsedQuery.computeRelationName);
    table->setReadWriteZero();
    if (table->compute())
    {
        Table *resultTable = new Table(parsedQuery.computeRelationName + "_RESULT");
        table->isMatrix = true;
        resultTable->blockCount = table->blockCount;
        resultTable->matDimensions = table->matDimensions;
        tableCatalogue.insertTable(resultTable);
    }
    table->getBlockStatistics();
    return;
}
