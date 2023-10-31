#include "global.h"

/**
 * @brief
 * SYNTAX: EXPORT <relation_name>
 */

bool syntacticParseEXPORT()
{
    logger.log("syntacticParseEXPORT");
    if (tokenizedQuery.size() < 2 and tokenizedQuery.size() > 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if (tokenizedQuery.size() == 3 and tokenizedQuery[1] == "MATRIX")
    {
        parsedQuery.queryType = EXPORT_MATRIX;
        parsedQuery.exportMatrixRelationName = tokenizedQuery[2];
    }
    else if (tokenizedQuery.size() == 2)
    {
        parsedQuery.queryType = EXPORT;
        parsedQuery.exportRelationName = tokenizedQuery[1];
    }
    return true;
}

bool semanticParseEXPORT()
{
    logger.log("semanticParseEXPORT");
    // Table should exist
    if (tableCatalogue.isTable(parsedQuery.exportRelationName) or tableCatalogue.isTable(parsedQuery.exportMatrixRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}

void executeEXPORT()
{
    logger.log("executeEXPORT");
    Table *table = tableCatalogue.getTable(parsedQuery.exportRelationName);
    table->makePermanent();
    return;
}

void executeEXPORTMATRIX()
{
    logger.log("executeEXPORTMATRIX");
    Table *table = tableCatalogue.getTable(parsedQuery.exportMatrixRelationName);
    table->setReadWriteZero();
    table->makeMatrixPermanent();
    table->getBlockStatistics();
    return;
}