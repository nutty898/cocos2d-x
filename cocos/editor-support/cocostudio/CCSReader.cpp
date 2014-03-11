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
        bool result;
        do
        {
            CC_BREAK_IF(!readJson(fileName, jsonDict));
            _node = createObject(jsonDict, nullptr);
        } while (0);

        if(jsonDict.HasParseError())
        {
            CCLOG("Json Parse Error: %s", jsonDict.GetParseError());
        }

        return _node;
    }

    Node *CCSReader::getNodeByTag(int nTag)
    {
        if (_node == nullptr)
        {
            return nullptr;
        }
        if (_node->getTag() == nTag)
        {
            return _node;
        }
        return nodeByTag(_node, nTag);
    }


    cocos2d::Node *CCSReader::nodeByTag(cocos2d::Node *parent, int tag)
    {
        if (parent == nullptr)
        {
            return nullptr;
        }
        Node *_retNode = nullptr;
        Vector<Node*>& Children = parent->getChildren();
        Vector<Node*>::iterator iter = Children.begin();
        while (iter != Children.end())
        {
            Node* pNode = *iter;
            if(pNode != nullptr && pNode->getTag() == tag)
            {
                _retNode =  pNode;
                break;
            }
            else
            {
                _retNode = nodeByTag(pNode, tag);
                if (_retNode != nullptr)
                {
                    break;
                }

            }
            ++iter;
        }
        return _retNode;
    }

    Node* CCSReader::createObject(const rapidjson::Value &dict, cocos2d::Node *parent)
    {
        //
        Node* newNode = nullptr;
        const char *className = DICTOOL->getStringValue_json(dict, "classname");
        if(strcmp(className, "Node") == 0)
        {
            newNode = Node::create();
        }
        else if(strcmp(className, "Scene") == 0)
        {
            newNode = Node::create();
        }
        else if(strcmp(className, "Sprite") == 0)
        {
            const char* fileName = DICTOOL->getStringValue_json(dict, "filename");
            
            if (fileName)
            {
                newNode = Sprite::create(fileName);
            }
            else
            {
                newNode = Sprite::create();
            }
        }
        
        //Add to parent
        if (parent != nullptr)
        {
            parent->addChild(newNode);
        }
        
        //Set properties
        setPropertyFromJsonDict(dict, newNode);
        
        //Get children
        int length = DICTOOL->getArrayCount_json(dict, "Children");
        for (int i = 0; i < length; ++i)
        {
            const rapidjson::Value &subDict = DICTOOL->getSubDictionary_json(dict, "Children", i);
            if (!DICTOOL->checkObjectExist_json(subDict))
            {
                break;
            }
            createObject(subDict, newNode);
        }
        
        return newNode;
    }
    
    void CCSReader::setPropertyFromJsonDict(const rapidjson::Value &root, cocos2d::Node *node)
    {
        const char* cx = DICTOOL->getStringValue_json(root, "x");
        const char* cy = DICTOOL->getStringValue_json(root, "y");
        float x = atof(cx);
        float y = atof(cy);
        node->setPosition(Point(x, y));
        
        const bool bVisible = DICTOOL->getBooleanValue_json(root, "visible");
        node->setVisible(bVisible);
        
        int nTag = DICTOOL->getIntValue_json(root, "tag", -1);
        node->setTag(nTag);
        
//        int nZorder = DICTOOL->getIntValue_json(root, "zorder");
//        node->setLocalZOrder(nZorder);
        
        const char* cScaleX = DICTOOL->getStringValue_json(root, "scalex");
        const char* cScaleY = DICTOOL->getStringValue_json(root, "scaley");
        float fScaleX = atof(cScaleX);
        float fScaleY = atof(cScaleY);
        node->setScaleX(fScaleX);
        node->setScaleY(fScaleY);
        
        const char* cSkewX = DICTOOL->getStringValue_json(root, "skewx");
        const char* cSkewY = DICTOOL->getStringValue_json(root, "skewy");
        float fSkewX = atof(cSkewX);
        float fSkewY = atof(cSkewY);
        node->setSkewX(fSkewX);
        node->setSkewY(fSkewY);
        
        const char* cRotation = DICTOOL->getStringValue_json(root, "rotation");
        float fRotationZ = atof(cRotation);
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