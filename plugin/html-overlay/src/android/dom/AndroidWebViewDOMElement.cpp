/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/Common.hpp"
#include "minko/input/Mouse.hpp"
#include "minko/dom/AbstractDOMEvent.hpp"
#include "android/dom/AndroidWebViewDOMElement.hpp"
#include "android/dom/AndroidWebViewDOMEngine.hpp"
#include "android/dom/AndroidWebViewDOM.hpp"
#include "android/dom/AndroidWebViewDOMTouchEvent.hpp"

#include <android/log.h>
#define LOG_TAG "MINKOTEST"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using namespace minko;
using namespace minko::dom;
using namespace android;
using namespace android::dom;

int
AndroidWebViewDOMElement::_elementUid = 0;

std::list<AndroidWebViewDOMElement::Ptr>
AndroidWebViewDOMElement::domElements;

std::map<std::string,AndroidWebViewDOMElement::Ptr>
AndroidWebViewDOMElement::_accessorToElement;

AndroidWebViewDOMElement::AndroidWebViewDOMElement(std::string jsAccessor) :
	_jsAccessor(jsAccessor),
	_onclick(Signal<AbstractDOMMouseEvent::Ptr>::create()),
	_onmousedown(Signal<AbstractDOMMouseEvent::Ptr>::create()),
	_onmousemove(Signal<AbstractDOMMouseEvent::Ptr>::create()),
	_onmouseup(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _onmouseout(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _onmouseover(Signal<AbstractDOMMouseEvent::Ptr>::create()),
    _onclickSet(false),
    _onmousedownSet(false),
    _onmousemoveSet(false),
    _onmouseupSet(false),
    _onmouseoverSet(false),
    _onmouseoutSet(false),
    _ontouchdown(Signal<AbstractDOMTouchEvent::Ptr>::create()),
    _ontouchup(Signal<AbstractDOMTouchEvent::Ptr>::create()),
    _ontouchmotion(Signal<AbstractDOMTouchEvent::Ptr>::create()),
    _ontouchdownSet(false),
    _ontouchupSet(false),
    _ontouchmotionSet(false),
    _engine(nullptr)
{
}

void
AndroidWebViewDOMElement::initialize(std::shared_ptr<AndroidWebViewDOMEngine> engine)
{
    _engine = engine;
    
    LOGI("Initialize: ");
    LOGI(_jsAccessor.c_str());

    std::string jsEval = _jsAccessor + ".minkoName = '" + _jsAccessor + "';";
    _engine->eval(jsEval);
}


AndroidWebViewDOMElement::Ptr
AndroidWebViewDOMElement::getDOMElement(std::string jsElement, std::shared_ptr<AndroidWebViewDOMEngine> engine)
{
	std::string js = "if (" + jsElement + ".minkoName) (" + jsElement + ".minkoName); else ('');";
	std::string minkoName = std::string(engine->eval(js.c_str()));

	if (minkoName == "")
	{
		minkoName = "Minko.element" + std::to_string(_elementUid++);

		js = minkoName + " = " + jsElement;
		engine->eval(js);
	}
	else
	{
		auto i = _accessorToElement.find(minkoName);

		if (i != _accessorToElement.end())
			return i->second;
	}

	return create(minkoName, engine);
}

AndroidWebViewDOMElement::Ptr
AndroidWebViewDOMElement::create(std::string jsAccessor, std::shared_ptr<AndroidWebViewDOMEngine> engine)
{
	AndroidWebViewDOMElement::Ptr element(new AndroidWebViewDOMElement(jsAccessor));
    element->initialize(engine);
    
    domElements.push_back(element);
	_accessorToElement[jsAccessor] = element;
    
	return element;
}

std::string
AndroidWebViewDOMElement::getJavascriptAccessor()
{
	return _jsAccessor;
}

std::string
AndroidWebViewDOMElement::id()
{
	std::string js = "(" + _jsAccessor + ".id)";
    std::string result = _engine->eval(js);
    
    return result;
}

void
AndroidWebViewDOMElement::id(std::string newId)
{
	std::string js = _jsAccessor + ".id = '" + newId + "';";
	_engine->eval(js);
}

std::string
AndroidWebViewDOMElement::className()
{
	std::string js = "(" + _jsAccessor + ".className)";
    std::string result = _engine->eval(js);
	
    return result;
}

void
AndroidWebViewDOMElement::className(std::string newClassName)
{
	std::string js = _jsAccessor + ".className = '" + newClassName + "';";
	_engine->eval(js);
}

std::string
AndroidWebViewDOMElement::tagName()
{
	std::string js = "(" + _jsAccessor + ".tagName)";
	std::string result = _engine->eval(js);
	
    return result;
}

AbstractDOMElement::Ptr
AndroidWebViewDOMElement::parentNode()
{
	std::string js = "Minko.tmpElement  = " + _jsAccessor + ".parentNode;";
	_engine->eval(js);

	return AndroidWebViewDOMElement::getDOMElement("Minko.tmpElement", _engine);
}

std::vector<minko::dom::AbstractDOMElement::Ptr>
AndroidWebViewDOMElement::childNodes()
{
	return (_engine->currentDOM()->getElementList(_jsAccessor + ".childNodes"));
}

std::string
AndroidWebViewDOMElement::textContent()
{
	std::string js = "(" + _jsAccessor + ".textContent)";
	std::string result = _engine->eval(js);
	
    return result;
}

void
AndroidWebViewDOMElement::textContent(std::string newTextContent)
{
	std::string js = _jsAccessor + ".textContent = '" + newTextContent + "';";
	_engine->eval(js);
}

std::string
AndroidWebViewDOMElement::innerHTML()
{
	std::string js = "(" + _jsAccessor + ".innerHTML)";
	std::string result = _engine->eval(js);
	
    return result;
}

void
AndroidWebViewDOMElement::innerHTML(std::string newInnerHTML)
{
	std::string js = _jsAccessor + ".innerHTML = '" + newInnerHTML + "';";
	_engine->eval(js);
}

AbstractDOMElement::Ptr
AndroidWebViewDOMElement::appendChild(minko::dom::AbstractDOMElement::Ptr newChild)
{
	AndroidWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<AndroidWebViewDOMElement>(newChild);
	std::string js = _jsAccessor + ".appendChild(" + child->getJavascriptAccessor() + ");";
	_engine->eval(js);

	return shared_from_this();
}

AbstractDOMElement::Ptr
AndroidWebViewDOMElement::removeChild(minko::dom::AbstractDOMElement::Ptr childToRemove)
{
	AndroidWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<AndroidWebViewDOMElement>(childToRemove);
	std::string js = _jsAccessor + ".removeChild(" + child->getJavascriptAccessor() + ");";
	_engine->eval(js);

	return shared_from_this();
}

AbstractDOMElement::Ptr
AndroidWebViewDOMElement::insertBefore(minko::dom::AbstractDOMElement::Ptr newChild, minko::dom::AbstractDOMElement::Ptr adjacentNode)
{
	AndroidWebViewDOMElement::Ptr child = std::dynamic_pointer_cast<AndroidWebViewDOMElement>(newChild);
	AndroidWebViewDOMElement::Ptr adjNode = std::dynamic_pointer_cast<AndroidWebViewDOMElement>(adjacentNode);
	std::string js = _jsAccessor + ".removeChild(" + child->getJavascriptAccessor() + ", " + adjNode->getJavascriptAccessor() + ");";
	_engine->eval(js);

	return shared_from_this();
}

AbstractDOMElement::Ptr
AndroidWebViewDOMElement::cloneNode(bool deep)
{
	std::string js = "Minko.tmpElement = " + _jsAccessor + ".cloneNode(" + (deep ? "true" : "false") + ");";
	_engine->eval(js);

	return AndroidWebViewDOMElement::create("Minko.tmpElement", _engine);
}

std::string
AndroidWebViewDOMElement::getAttribute(std::string name)
{
	std::string js = "(" + _jsAccessor + ".getAttribute('" + name + "'))";
	std::string result = _engine->eval(js);
	
    return result;
}

void
AndroidWebViewDOMElement::setAttribute(std::string name, std::string value)
{
	std::string js = _jsAccessor + ".setAttribute('" + name + "', '" + value + "');";
	_engine->eval(js);
}

std::vector<minko::dom::AbstractDOMElement::Ptr>
AndroidWebViewDOMElement::getElementsByTagName(std::string tagName)
{
	return (_engine->currentDOM()->getElementList(_jsAccessor + ".getElementsByTagName('" + tagName + "')"));
}

std::string
AndroidWebViewDOMElement::style(std::string name)
{
	std::string js = "(" + _jsAccessor + ".style." + name + ")";
	std::string result = _engine->eval(js);
	
    return result;
}

void
AndroidWebViewDOMElement::style(std::string name, std::string value)
{
	std::string js = _jsAccessor + ".style." + name + " = '" + value + "';";
	_engine->eval(js);
}

void
AndroidWebViewDOMElement::addEventListener(std::string type)
{
	std::string js = "Minko.addListener(" + _jsAccessor + ", '" + type + "');";

	_engine->eval(js);
}

// Events

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AndroidWebViewDOMElement::onclick()
{
	if (!_onclickSet)
	{
		addEventListener("click");
		_onclickSet = true;
	}

	return _onclick;
}

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AndroidWebViewDOMElement::onmousedown()
{
	if (!_onmousedownSet)
	{
		addEventListener("mousedown");
		_onmousedownSet = true;
	}

	return _onmousedown;
}

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AndroidWebViewDOMElement::onmouseup()
{
    if (!_onmouseupSet)
    {
        addEventListener("mouseup");
        _onmouseupSet = true;
    }
    
    return _onmouseup;
}


Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AndroidWebViewDOMElement::onmousemove()
{
	if (!_onmousemoveSet)
	{
		addEventListener("mousemove");
		_onmousemoveSet = true;
	}
	
	return _onmousemove;
}

Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AndroidWebViewDOMElement::onmouseout()
{
    if (!_onmouseoutSet)
    {
        addEventListener("mouseout");
        _onmouseoutSet = true;
    }
    
    return _onmouseout;
}


Signal<std::shared_ptr<AbstractDOMMouseEvent>>::Ptr
AndroidWebViewDOMElement::onmouseover()
{
	if (!_onmouseoverSet)
	{
		addEventListener("mouseover");
		_onmouseoverSet = true;
	}

	return _onmouseover;
}

Signal<std::shared_ptr<AbstractDOMTouchEvent>>::Ptr
AndroidWebViewDOMElement::ontouchdown()
{
    if (!_ontouchdownSet)
    {
        addEventListener("touchstart");
        _ontouchdownSet = true;
    }
    
    return _ontouchdown;
}

Signal<std::shared_ptr<AbstractDOMTouchEvent>>::Ptr
AndroidWebViewDOMElement::ontouchup()
{
    if (!_ontouchupSet)
    {
        addEventListener("touchend");
        _ontouchupSet = true;
    }
    
    return _ontouchup;
}

Signal<std::shared_ptr<AbstractDOMTouchEvent>>::Ptr
AndroidWebViewDOMElement::ontouchmotion()
{
    if (!_ontouchmotionSet)
    {
        addEventListener("touchmove");
        _ontouchmotionSet = true;
    }
    
    return _ontouchmotion;
}

void
AndroidWebViewDOMElement::update()
{
    if (_engine->isReady())
    {
        
        std::string js = "(Minko.getEventsCount(" + _jsAccessor + "));";
        int l = atoi(_engine->eval(js).c_str());

        for(int i = 0; i < l; ++i)
        {
            std::string eventName = "Minko.event" + std::to_string(_elementUid++);
            js =  eventName + " = " + _jsAccessor + ".minkoEvents[" + std::to_string(i) + "];";
            _engine->eval(js);
                       
            // It's a touch event ?
            if (_engine->eval(eventName + ".type").find("touch") == 0)
            {
                // Get number of finger
                std::string js = eventName + ".changedTouches.length";
                int touchNumber = atoi(_engine->eval(js).c_str());
                
                for (auto i = 0; i < touchNumber; i++)
                {
                       
                    // Get the finger id (note: JS events can send identifier > INT_MAX, that's why there is a modulo)
                    js = "((" + eventName + ".changedTouches[" + std::to_string(i) + "].identifier));";

                    int fingerId = atoi(_engine->eval(js).c_str());

                    // Create the touch event
                    AndroidWebViewDOMTouchEvent::Ptr event = AndroidWebViewDOMTouchEvent::create(eventName, fingerId, i, _engine);
                    
                    std::string type = event->type();
                    
                    if (type == "touchstart")
                    {
                        _ontouchdown->execute(event);
                        
                        // If it's the first finger
                        if (_engine->touchNumber() == 1)
                        {
                            // Set the first finger id
                            _engine->firstFingerId(fingerId);
                            
                            _onmousedown->execute(event);
                        }
                    }
                    else if (type == "touchend")
                    {
                        _ontouchup->execute(event);
                        
                        // If it's the first finger
                        if (fingerId == _engine->firstFingerId())
                        {
                            _engine->firstFingerId(-1);
                            
                            _onclick->execute(event);
                            _onmouseup->execute(event);
                        }
                    }
                    else if (type == "touchmove")
                    {
                        _ontouchmotion->execute(event);
                        
                        // If it's the first finger
                        if (fingerId == _engine->firstFingerId())
                        {
                            _onmousemove->execute(event);
                        }
                    }
                }
            }
        }

        js = "(Minko.clearEvents(" + _jsAccessor + "));";
        _engine->eval(js);
    }
}