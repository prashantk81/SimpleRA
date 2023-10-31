#include "global.h"
/**
 * @brief
 * SYNTAX: LOAD relation_name
 * SYNTAX: LOAD MATRIX matrix_name
 */
bool syntacticParseLOAD()
{
    logger.log("syntacticParseLOAD");
    if (tokenizedQuery.size() < 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if (tokenizedQuery.size() == 2)
    {
        parsedQuery.queryType = LOAD;
        parsedQuery.loadRelationName = tokenizedQuery[1];
    }
    else if (tokenizedQuery.size() == 3)
    {
        parsedQuery.queryType = LOAD_MATRIX;
        parsedQuery.loadRelationName = tokenizedQuery[2];
    }
    return true;
}

bool semanticParseLOAD()
{
    logger.log("semanticParseLOAD");
    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD()
{
    logger.log("executeLOAD");

    Table *table = new Table(parsedQuery.loadRelationName);
    if (table->load())
    {
        tableCatalogue.insertTable(table);
        cout << "Loaded Table. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
    }
    return;
}

void executeLOADMATRIX()
{
    logger.log("executeLOADMATRIX");

    Table *table = new Table(parsedQuery.loadRelationName);
    table->setReadWriteZero();
    if (table->loadMatrix())
    {
        table->isMatrix = true;
        tableCatalogue.insertTable(table);
    }
    table->getBlockStatistics();
    return;
}