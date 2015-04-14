/*****************************************************************************/
/*!
\file		DefinitionTable.cpp
\author 	Gavin Yeo Wei Ming, gavin.yeo, 290000111
\par    	email: gavin.yeo\@digipen.edu
\date   	March 10, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/DefinitionTable.h"
#include "Simian/Debug.h"
#include <stdlib.h>
#include <sstream>

namespace Simian
{
    static std::string EMPTY = "";

    // Enumeration for Comments State Machine
    enum COMMENTS_STATE_MACHINE
    { NOT_IN_COMMENT,  // Currently not in comment
    FIRST_SLASH_MET, // Has met 1st slash of comment
    FIRST_AST_MET,   // Has met 1st asterisk of comment
    SECOND_AST_MET,  // Has met 2nd asterisk of comment
    CPLUS_MET        // Has met a C++ style comment
    };
    
    const std::string& DefinitionTable::Key(s32 value) const
    {
        for (std::map<std::string, s32>::const_iterator i = idTables_.begin(); i != idTables_.end(); ++i)
            if ((*i).second == value)
                return (*i).first;
        SAssert(false, "The name does not exist in the definition table.");
        return EMPTY;
    }

    // Check the enumName from IDTable and return the value
    const s32 DefinitionTable::Value(const std::string& enumName) const
    {
        std::map<std::string, s32>::const_iterator i = idTables_.find(enumName);
        SAssert(i != idTables_.end(), "The name does not exist in the definition table.");
        return i->second;
    }

    // Check if there is a value like this
    bool DefinitionTable::HasValue(const std::string& enumName) const
    {
        std::map<std::string, s32>::const_iterator i = idTables_.find(enumName);
        return i != idTables_.end();
    }

    //--------------------------------------------------------------------------


    // Open the file and parse enum tags, save into IDTables
    void DefinitionTable::Load(const std::vector<DataLocation>& filePathVec)
    {
        // Loop through the vector of files
        for (u32 vc = 0; vc < filePathVec.size(); ++vc)
        {
            // Open file to read
            std::stringstream file;
            file << filePathVec[vc].Memory();
            SAssert(!file.fail(), "Failed to open file.");
            
            // Get strings from file
            s32 commState = NOT_IN_COMMENT;

            s32 prevVal = -1, currValue = 0;
            bool isReadingValue=false;
            s8 tempC, tempCommC;
            std::string tempS, enumName = "", currName = "", &rCurrName = currName;
            std::string prevName = "", &rPrevName = prevName;
            while (!file.eof( ))
            {
                file.get(tempC);
                
                // Check if currently in a comment
                if (InCommentState(&tempC, &tempCommC, &commState))
                    continue;

                // Look for word "enum"
                if (tempC=='e' && enumName=="" && commState==NOT_IN_COMMENT)
                {
                    getline(file, tempS, ' ');
                    // Entered enum
                    if (tempS=="num")
                    {
                        // Loop to determine overall enum name
                        while (tempC!=' ' && tempC!='{')
                        {
                            file.get(tempC);
                            if (tempC != ' ')
                                enumName += tempC;
                        }


                        // Loop to determine member names
                        while (tempC!='}')
                        {
                            file.get(tempC);

                            // Check if currently in a comment
                            if (InCommentState(&tempC, &tempCommC, &commState))
                                continue;

                            if (tempC!=' ' && tempC!='=' && tempC!='{' && tempC!='\n' && tempC!='\r' && tempC!='\t')
                            {
                                // Currently reading value
                                if (isReadingValue == true)
                                {
                                    tempS = "";
                                    while (tempC!=' ' && tempC!=',' && tempC!='}' && tempC!='/')
                                    {
                                        tempS += tempC;
                                        file.get(tempC);
                                    }
                                    // Search through if it is a string
                                    std::map<std::string, s32>::iterator itString = idTables_.find(tempS);
                                    if (itString != idTables_.end())
                                    {
                                        currValue = itString->second;
                                        idTables_.erase(tempS);
                                    }
                                    else
                                        currValue = atoi(tempS.c_str());
                                    prevVal = currValue;
                                    isReadingValue = false;
                                    idTables_[rPrevName] = currValue;
                                }
                                else
                                {
                                    // If value is not provided for current name
                                    if (tempC==',')
                                    {
                                        currValue = ++prevVal;
                                        idTables_[rCurrName] = currValue;
                                    }
                                    else if (tempC != '}')
                                        // Push characters into member name
                                        currName += tempC;
                                    else if (tempC == '}')
                                    {
                                        SetMemberName(rCurrName, rPrevName);
                                    }
                                }
                            }
                            else
                            {
                                // Equal sign means user is setting value to current name
                                if (tempC=='=')
                                {
                                    isReadingValue = true;
                                    SetMemberName(rCurrName, rPrevName);
                                }
                                if (rCurrName != "" || tempC=='}')
                                    SetMemberName(rCurrName, rPrevName);
                            }
                        } // End while check for '}'
                        // Final value set if the last name did not have value provided
                        if (idTables_.find(rPrevName) != idTables_.end() && idTables_[rPrevName]==-1)
                        {
                            currValue = ++prevVal;
                            idTables_[rPrevName] = currValue;

                        }
                    }
                }
            }
        }
    }

    // Store string as tag in IDTable map
    void DefinitionTable::SetMemberName(std::string& currName, std::string& prevName)
    {
        if (currName != "")
        {
            if (!idTables_[currName] && prevName != "")
            {
                if (currName[0] != '/')
                {
                    idTables_[currName] = -1;
                }
            }
            else
            {
                if (currName[0] != '/')
                {
                    idTables_[currName];
                }
            }
            prevName = currName;
            currName = "";
        }
    }

    // Check current state of comment
    bool DefinitionTable::InCommentState(const s8* currChar, s8* tempCommC, s32* commState)
    {
        // Check for comments
        if (*currChar=='/' && *commState==NOT_IN_COMMENT)
        {
            *tempCommC = *currChar;
            *commState = FIRST_SLASH_MET;
        }
        // First asterisk met for C style comments
        else if (*currChar=='*' && *commState==FIRST_SLASH_MET)
            *commState = FIRST_AST_MET;
        // 2nd slash met for C++ style comments
        else if (*currChar=='/' && *commState==FIRST_SLASH_MET)
            *commState = CPLUS_MET;
        else if ((*currChar!='*' || *currChar!='/') && *commState==FIRST_SLASH_MET)
            *commState = NOT_IN_COMMENT;
        else if (*currChar=='*' && *commState==FIRST_AST_MET)
            *commState = SECOND_AST_MET;
        else if (*currChar=='\n' && *commState==CPLUS_MET)
            *commState = NOT_IN_COMMENT;
        if (*currChar!='*' && *currChar!='/' && *commState==SECOND_AST_MET)
            *commState = FIRST_AST_MET;
        // End C style comment
        if (*currChar=='/' && *commState==SECOND_AST_MET)
            *commState = NOT_IN_COMMENT;
        // Continue if in comment
        if (*commState==CPLUS_MET || *commState==FIRST_AST_MET || *commState==FIRST_SLASH_MET)
            return true;
        else
            return false;
    }
}
