/****************************************************************************
 Copyright (c) 2013-2014 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#ifndef __CCS_READER_H__
#define __CCS_READER_H__

#include "cocostudio/DictionaryHelper.h"
#include "cocos2d.h"

namespace cocostudio {
    
class CCSReader
{
public:
    static CCSReader* getInstance();
    cocos2d::Node* loadSceneFile(const std::string& fileName);
	cocos2d::Node* getNodeByTag(int nTag);
    
private:
    CCSReader();
    virtual ~CCSReader();
    
    cocos2d::Node* createObject(const rapidjson::Value& dict, cocos2d::Node* parent);
    void setPropertyFromJsonDict(const rapidjson::Value& dict, cocos2d::Node *node);
    bool readJson(const std::string& fileName, rapidjson::Document& doc);
    
private:
    static CCSReader* s_sharedReader;
    
    cocos2d::Node* _node;
};
    
}

#endif
