/****************************************************************************
Copyright (c) 2013 cocos2d-x.org

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

#include "CCNodeCache.h"
#include "CCTimelineActionCache.h"
#include "CCFrame.h"

using namespace cocos2d;

namespace cocostudio {
namespace timeline{


static const char* NodeType_Node     = "Node";
static const char* NodeType_SubGraph = "SubGraph";
static const char* NodeType_Sprite   = "Sprite";
static const char* NodeType_Particle = "Particle";


static const char* NODE        = "nodeTree";
static const char* CHILDREN    = "children";
static const char* NODETYPE    = "classname";
static const char* FILE_PATH   = "filePath";
static const char* ACTION_TAG  = "actionTag";

static const char* OPTIONS      = "options";

static const char* X                = "x";
static const char* Y                = "y";
static const char* SCALE_X          = "scaleX";
static const char* SCALE_Y          = "scaleY";
static const char* SKEW_X           = "skewX";
static const char* SKEW_Y           = "skewY";
static const char* ROTATION         = "rotation";
static const char* ROTATION_SKEW_X  = "rotationSkewX";
static const char* ROTATION_SKEW_Y  = "rotationSkewY";
static const char* ANCHOR_X         = "anchorPointX";
static const char* ANCHOR_Y         = "anchorPointY";
static const char* ALPHA            = "opacity";
static const char* RED              = "colorR";
static const char* GREEN            = "colorG";
static const char* BLUE             = "colorB";

static NodeCache* _sharedNodeCache = nullptr;

NodeCache* NodeCache::getInstance()
{
    if (! _sharedNodeCache)
    {
        _sharedNodeCache = new NodeCache();
        _sharedNodeCache->init();
    }

    return _sharedNodeCache;
}

void NodeCache::destroyInstance()
{
    CC_SAFE_DELETE(_sharedNodeCache);
}

void NodeCache::purge()
{
    _nodes.clear();
}

void NodeCache::init()
{
    using namespace std::placeholders;
    _funcs.insert(Pair(NodeType_Node,       std::bind(&NodeCache::loadSimpleNode, this, _1)));
    _funcs.insert(Pair(NodeType_SubGraph,   std::bind(&NodeCache::loadSubGraph,   this, _1)));
    _funcs.insert(Pair(NodeType_Sprite,     std::bind(&NodeCache::loadSprite,     this, _1)));
    _funcs.insert(Pair(NodeType_Particle,   std::bind(&NodeCache::loadParticle,   this, _1)));
}

cocos2d::Node* NodeCache::createNode(const std::string& filename)
{
    cocos2d::Node* node = _nodes.at(filename);
    if (node == nullptr)
    {
        node = loadNodeWithFile(filename);

//         if(cache)
//             _nodes.insert(filename, node);
    }

    return node;
}

cocos2d::Node* NodeCache::loadNodeWithFile(const std::string& fileName)
{
    // Read content from file
    std::string fullPath = CCFileUtils::getInstance()->fullPathForFilename(fileName);
    std::string contentStr = FileUtils::getInstance()->getStringFromFile(fullPath);

    // Load animation data from file
    TimelineActionCache::getInstance()->loadAnimationActionWithContent(fileName, contentStr);

    return loadNodeWithContent(contentStr);
}

cocos2d::Node* NodeCache::loadNodeWithContent(const std::string& content)
{
    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if (doc.HasParseError()) {
        CCLOG("GetParseError %s\n", doc.GetParseError());
    }

    const rapidjson::Value& subJson = DICTOOL->getSubDictionary_json(doc, NODE);
    return loadNode(subJson);
}

cocos2d::Node* NodeCache::loadNode(const rapidjson::Value& json)
{
    cocos2d::Node* node = nullptr;

    std::string nodeType = DICTOOL->getStringValue_json(json, NODETYPE);

    NodeCreateFunc func = _funcs.at(nodeType);
    if (func != nullptr)
    {
        const rapidjson::Value &options = DICTOOL->getSubDictionary_json(json, OPTIONS);
        node = func(options);
    }

    int tag = DICTOOL->getIntValue_json(json, ACTION_TAG);
    node->setTag(tag);

    int length = DICTOOL->getArrayCount_json(json, CHILDREN, 0);
    for (int i = 0; i<length; i++)
    {
        const rapidjson::Value &dic = DICTOOL->getSubDictionary_json(json, CHILDREN, i);
        cocos2d::Node* child = loadNode(dic);
        node->addChild(child);
    }

    return node;
}

void NodeCache::initNode(cocos2d::Node* node, const rapidjson::Value& json)
{
    float x             = DICTOOL->getFloatValue_json(json, X);
    float y             = DICTOOL->getFloatValue_json(json, Y);
    float scalex        = DICTOOL->getFloatValue_json(json, SCALE_X, 1);
    float scaley        = DICTOOL->getFloatValue_json(json, SCALE_Y, 1);
    float rotation      = DICTOOL->getFloatValue_json(json, ROTATION);
    float rotationSkewX = DICTOOL->getFloatValue_json(json, ROTATION_SKEW_X);
    float rotationSkewY = DICTOOL->getFloatValue_json(json, ROTATION_SKEW_X);
    float skewx         = DICTOOL->getFloatValue_json(json, SKEW_X);
    float skewy         = DICTOOL->getFloatValue_json(json, SKEW_Y);
    float anchorx       = DICTOOL->getFloatValue_json(json, ANCHOR_X, 0.5f);
    float anchory       = DICTOOL->getFloatValue_json(json, ANCHOR_Y, 0.5f);
    GLubyte alpha       = (GLubyte)DICTOOL->getIntValue_json(json, ALPHA, 255);
    GLubyte red         = (GLubyte)DICTOOL->getIntValue_json(json, RED, 255);
    GLubyte green       = (GLubyte)DICTOOL->getIntValue_json(json, GREEN, 255);
    GLubyte blue        = (GLubyte)DICTOOL->getIntValue_json(json, BLUE, 255);

    if(x != 0 || y != 0)
        node->setPosition(Point(x, y));
    if(scalex != 1)
        node->setScaleX(scalex);
    if(scaley != 1)
        node->setScaleY(scaley);
    if (rotation != 0)
        node->setRotation(rotation);
    if(rotationSkewX != 0)
        node->setRotationSkewX(rotationSkewX);
    if(rotationSkewY != 0)
        node->setRotationSkewY(rotationSkewY);
    if(skewx != 0)
        node->setSkewX(skewx);
    if(skewy != 0)
        node->setSkewY(skewy);
    if(anchorx != 0.5f || anchory != 0.5f)
        node->setAnchorPoint(Point(anchorx, anchory));
    if(alpha != 255)
        node->setOpacity(alpha); node->setCascadeOpacityEnabled(true);
    if(red != 255 || green != 255 || blue != 255)
        node->setColor(Color3B(red, green, blue));
}


Node* NodeCache::loadSimpleNode(const rapidjson::Value& json)
{
    Node* node = Node::create();
    initNode(node, json);

    return node;
}

cocos2d::Node* NodeCache::loadSubGraph(const rapidjson::Value& json)
{
    const char* filePath = DICTOOL->getStringValue_json(json, FILE_PATH);

    Node* node = nullptr;
    if(filePath)
    {
        node = createNode(filePath);
    }
    else
    {
        node = Node::create();
    }

    initNode(node, json);

    return node;
}

Node* NodeCache::loadSprite(const rapidjson::Value& json)
{
    const char* filePath = DICTOOL->getStringValue_json(json, FILE_PATH);

    Sprite *sprite = nullptr;
    SpriteFrame* spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(filePath);
    if(!spriteFrame)
    {
        sprite = Sprite::create(filePath);
    }
    else
    {
        sprite = Sprite::createWithSpriteFrame(spriteFrame);
    }

    if(!sprite)
        CCLOG("create sprite with file name : %s  failed.", filePath);

    initNode(sprite, json);

    return sprite;
}

Node* NodeCache::loadParticle(const rapidjson::Value& json)
{
    return nullptr;
}

}
}
