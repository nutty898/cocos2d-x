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

#include "CCSReader.h"

using namespace cocos2d;

namespace cocostudio {

    CCSReader* CCSReader::s_sharedReader = nullptr;

    CCSReader::CCSReader()
    {
        //
    }

    CCSReader::~CCSReader()
    {
        //
    }
    
    CCSReader* CCSReader::getInstance()
    {
        if(s_sharedReader == nullptr)
        {
            s_sharedReader = new CCSReader();
        }
        return s_sharedReader;
    }
    
    Node* CCSReader::loadSceneFile(const std::string &fileName)
    {
        rapidjson::Document jsonDict;
        do
        {
            CC_BREAK_IF(!readJson(fileName, jsonDict));
            _node = createObject(jsonDict, nullptr);
        } while (0);
        
        return _node;
    }
    
    Node* CCSReader::createObject(const rapidjson::Value &dict, cocos2d::Node *parent)
    {
        //
        const char *className = DICTOOL->getStringValue_json(dict, "classname");
        if(strcmp(className, "Node") == 0)
        {
            Node* gb = nullptr;
            if (parent == nullptr)
            {
                gb = Node::create();
            }
            else
            {
                gb = Node::create();
                parent->addChild(gb);
            }
            
            setPropertyFromJsonDict(dict, gb);
            
            return gb;
        }
        
        return nullptr;
    }
    
    void CCSReader::setPropertyFromJsonDict(const rapidjson::Value &root, cocos2d::Node *node)
    {
        float x = DICTOOL->getFloatValue_json(root, "x");
        float y = DICTOOL->getFloatValue_json(root, "y");
        node->setPosition(Point(x, y));
        
        const bool bVisible = (DICTOOL->getIntValue_json(root, "visible", 1) != 0);
        node->setVisible(bVisible);
        
        int nTag = DICTOOL->getIntValue_json(root, "objecttag", -1);
        node->setTag(nTag);
        
        int nZorder = DICTOOL->getIntValue_json(root, "zorder");
        node->setLocalZOrder(nZorder);
        
        float fScaleX = DICTOOL->getFloatValue_json(root, "scalex", 1.0);
        float fScaleY = DICTOOL->getFloatValue_json(root, "scaley", 1.0);
        node->setScaleX(fScaleX);
        node->setScaleY(fScaleY);
        
        float fRotationZ = DICTOOL->getFloatValue_json(root, "rotation");
        node->setRotation(fRotationZ);
    }
    
    bool CCSReader::readJson(const std::string &fileName, rapidjson::Document &doc)
    {
        bool bRet = false;
        do {
            std::string jsonpath = FileUtils::getInstance()->fullPathForFilename(fileName);
            std::string contentStr = FileUtils::getInstance()->getStringFromFile(jsonpath);
            doc.Parse<0>(contentStr.c_str());
            CC_BREAK_IF(doc.HasParseError());
            bRet = true;
        } while (0);
        return bRet;
    }
}