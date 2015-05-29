/*Copyright (c) <2014> Kevin Miller - KevM1227@gmail.com

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#ifndef OCS_OBJECTMANAGER_H
#define OCS_OBJECTMANAGER_H

#include <map>
#include <queue>
#include <type_traits>
#include <utility>

#include <OCS/Components/Component.hpp>
#include <OCS/Misc/NonCopyable.hpp>
#include <OCS/Objects/Object.hpp>
#include <OCS/Components/ComponentArray.hpp>
#include <OCS/Misc/Config.hpp>
#include <OCS/Misc/TemplateMacros.hpp>
#include <OCS/Components/SentinalType.hpp>

namespace ocs
{

CREATE_HAS_MEMBER_FUNCTION(family, hasComponentFamily);

/**\brief Manages the lifetime of game objects. A blank object can be created
*         and components may be added manually, or alternatively, the user
*         may specify a custom prototype to copy the new object from.
*
* This class relies heavily on templates and static variables to achieve an effect where any type
* of component can be created and added to an object without the need to hard program
* anything in the manager.
* The objects themselves consist of an ID(their index in objects array),
* whether or not the object is a prototype, and two maps that store indicators of the object's
* components. These maps are called upon when destroying an object and copying a prototype to
* create a new object.
*
*@author Kevin Miller
*@version 2-22-2014
*/
class ObjectManager : NonCopyable
{
    public:

        //!Default constructor
        ObjectManager();
        ~ObjectManager();

        //!Add any components that the object doesn't have or set the value of a component to the given value if the object does have an instance of it
        template<typename C, typename ... Args>
        ID assign(ID, const C&, Args&& ...);

        /**Assign a component based on component name and deSerialization string
        Add any components that the object doesn't have or set the value of a component to the given value if the object does have an instance of it*/
        template<typename ... Args>
        ID assign(ID, const std::string&, const std::string&, Args&& ...);

        //!Add the given components to an object prototype under the specified name
        template<typename C, typename ... Args, typename = typename std::enable_if<hasComponentFamily<C>::value>::type>
        void assignToPrototype(const std::string&, const C&, Args&& ...);

        //!Add components to a prototype using strings. Used mainly for prototype file loading.
        template<typename ... Args>
        void assignToPrototype(const std::string&, const std::string&, const std::string&, Args&& ...);

        //!Allow a component to be referred to by a string
        template<typename C>
        void bindStringToComponent(const std::string&);

        //!Copy a game object
        void copyObject(ID, const Object&);

        //!Create a game object with no components
        ID createObject();

        //!Create a game object from an object prototype
        ID createObject(const std::string&);

        //!Create a game object from an object prototype (Overloaded to differentiate from template function)
        ID createObject(const char* str) { return createObject(std::string(str)); }

        //!Create a game object from one or more components
        template<typename C, typename ... Args>
        ID createObject(const C&, Args&& ...);

        //!Create a game object from an existing object
        ID createObject(const Object&);

        //Deserialize all of an object's components
        void deSerializeObject(ID, std::vector<std::pair<Family, std::string>>&);

        //!Destroy a game object from the object's ID
        void destroyObject(ID);

        //!Destroy all game objects
        void destroyAllObjects();

        //!Check if a prototype of the specified name exists
        bool doesPrototypeExist(const std::string&) const;

        //!Get a single component from the object's ID
        template<typename C>
        C* const getComponent(ID);

        //!Retrives the given component's array. If it does not exists, one is created and returned.
        template<typename C>
        ComponentArray<C>& getComponentArray() const;

        //!Returns a list of object ids that have the specified components
        template<typename ... Args>
        std::vector<ID> getObjects();

        //!Get a count of the specified component
        template<typename C>
        ID getTotalComponents() const;

        //!Get a count of all objects
        ID getTotalObjects() const;

        //!Check if an object has the specified prototype
        template<typename C = SentinalType, typename ... Args>
        bool hasComponents(ID);

        //!Check if a prototype has the specified prototype
        template<typename C = SentinalType, typename ... Args>
        bool hasComponentsPrototype(const std::string&);

        // //!Check if an object id is a prototype's id
        // bool isPrototype(ID);

        //!Must be called before using components
        template<typename C, typename ... Args>
        void registerComponents();

        //!Must be called before using components
        template<typename C>
        void registerComponent();

        //!Remove a component from the object's ID
        template<typename C = SentinalType, typename ... Args>
        ID remove(ID);

        //!Remove all components from the object's ID
        ID removeAll(ID);

        //!Remove the given components from an object prototype under the specified name
        template<typename C>
        void removeFromPrototype(const std::string&);

        //!Serialize all components of an object
        std::vector<std::string> serializeObject(ID);

    private:

        //!All game objects reside in here
        PackedArray<Object> objects;

        //!Stores an object prototype under its name for easy lookup
        std::unordered_map<std::string, Object> objectPrototypes;

        //!Stores a prototype base component array with an associated id
        std::unordered_map<ID, BaseComponentArray*> compFamilyToProtoCompArray;

        //!Stores a base component array with an associated id
        static std::unordered_map<ID, BaseComponentArray*> compFamilyToCompArray;

        //!Stores a component id with an associated string
        static std::unordered_map<std::string, ID> stringToCompFamily;

        //!Stores components for object prototypes
        template<typename C>
        ComponentArray<C>& getPrototypeComponentArray() const;

        //!Overload function with an empty template paramater list to allow recursion
        ID assign(ID) { return 0;}
        ID assignFromString(Object&, std::unordered_map<ID, BaseComponentArray*>&) { }

        //!Add a component from an exisiting component
        template<typename C, typename ... Args>
        ID addComponents(ID, const C&, Args&& ...);

        template<typename ... Args>
        ID assignFromString(Object&, std::unordered_map<ID, BaseComponentArray*>&, const std::string&, const std::string&, Args&& ...);

        //!Overload function with an empty template paramater list to allow recursion
        void assignToPrototype(const std::string&) { }

        //!Set a component from an existing component
        template<typename C>
        bool setComponent(ID, const C&);

        //!Set a component through the component's constructor arguments
        template<typename C, typename ... Args>
        bool setComponent(ID, Args&& ...);

        static ID prototypeIDCounter;

        //Used internally, so different instances of the ObjectManager can have different component arrays
        ID version;
        static ID versionCounter;
        static std::queue<ID> availableVersions;
};

template <>
template <typename C, typename ... Args>
void ObjectManager::registerComponents<void>() { }

/** \brief Return a reference to an array of the specified component.
 *
 * \return A reference to a component's array.
 */
template<typename C>
ComponentArray<C>& ObjectManager::getComponentArray() const
{
    static std::unordered_map<ID, ComponentArray<C>> components;

    return components[version];
}

/** \brief Query the object manager for a list of objects that have the specified components
 *
 * \return A vector of object ids with the specified components
 */
template<typename ... Args>
std::vector<ID> ObjectManager::getObjects()
{
    std::vector<ID> ids;
    for (auto& obj : objects)
    {
        if (hasComponents<Args...>(obj.objectID))
            ids.push_back(obj.objectID);
    }
    return ids;
}

/** \brief Return a reference to the arrays for prototype components.
 *
 * \return A reference to the prototype component array
 */
template<typename C>
ComponentArray<C>& ObjectManager::getPrototypeComponentArray() const
{
    static std::unordered_map<ID, ComponentArray<C>> components;

    return components[version];
}

/** \brief Create an object from one or more components. Only one instance of each component will be added to the object.
 *
 * \param component The first component to be added.
 * \param others The other components to be added.
 * \return The created object's id.
 *
 */
template<typename C, typename ... Args>
ID ObjectManager::createObject(const C& component, Args&& ... others)
{
    ID id = createObject();
    assign(id, component, others...);

    return id;
}

/** \brief Add the given components to the object with the specified id. Each object can only have one instance of each component.
 *
 * \param objectID The id of the object to add the components to.
 * \param component The first component to add.
 * \param others Any other components to add.
 * \return The total number of components that were added.
 *
 */
template<typename C, typename ... Args>
ID ObjectManager::addComponents(ID objectID, const C& component, Args&& ... others)
{
    registerComponents<C>();

    //A counter for the total components added to the object
    ID added = 0;

    if (objects.isValid(objectID))
    {
        //Only add the component if the object does not have an instance of it already.
        if (objects[objectID].componentIndices.find(C::componentFamily()) == objects[objectID].componentIndices.end())
        {
            //Add the component to its array
            ID componentIndex = getComponentArray<C>().addItem(component);
            getComponentArray<C>()[componentIndex].ownerID = objectID;

            //Store a pointer to the components PackedArray for destroying of the object
            objects[objectID].componentArrays[C::componentFamily()] = &getComponentArray<C>();

            //Store the component's index in the object
            objects[objectID].componentIndices[C::componentFamily()] = componentIndex;

            added = 1;
        }
    }

    //If there are more objects to add then add them.
    if (sizeof ... (others) > 0)
        return added + addComponents(objectID, others...);
    return added;
}

/** \brief Set the value of a component to the given value.
 *
 * \param objectID The owner object's id.
 * \param value The new value for the component.
 *
 */
template<typename C>
bool ObjectManager::setComponent(ID objectID, const C& value)
{
    if (objects.isValid(objectID))
    {
        auto compPtr = getComponent<C>(objectID);
        // If the object has the specified component
        if (compPtr)
        {
            //Set the objects component to the new value
            *compPtr = value;
            return true;
        }
    }

    return false;
}

/** \brief Add any components that the object doesn't have or set the value of a component to the given value if the object does have an instance of it
 *
 * \param objectID The owner object's id.
 * \param value The new value for the component.
 *
 */
template<typename C, typename ... Args>
ID ObjectManager::assign(ID objectID, const C& first, Args&& ... others)
{
    ID numAdded = 0;
    if (objects.isValid(objectID))
    {
        if (!hasComponents<C>(objectID))
            numAdded = assign(objectID, first);
        else
            setComponent(objectID, first);

        //If there are more objects to add then add them.
        if (sizeof...(others) > 0)
            return numAdded + assign(objectID, others...);
    }
    return numAdded;
}

/**Assign a component based on component name and deSerialization string
Add any components that the object doesn't have or set the value of a component to the given value if the object does have an instance of it*/
template<typename ... Args>
ID ObjectManager::assign(ID objectID, const std::string& componentName, const std::string& compValues, Args&& ... others)
{
    if (objects.isValid(objectID))
        return assignFromString(objects[objectID], compFamilyToCompArray, componentName, compValues, others...);
    return 0;
}

/** \brief Remove the specified components from object with the given id.
 *
 * \param objectID The owner object's id.
 * \return True if the component was removed. False if it was not removed.
 *
 */
template<typename C, typename ... Args>
ID ObjectManager::remove(ID objectID)
{
    ID totalRemoved = 0;
    if (!SentinalType::endRecursion(C()))
    {
        if (objects.isValid(objectID))
        {
            //Only remove the component if the object has an instance of it.
            if (objects[objectID].componentIndices.find(C::componentFamily()) != objects[objectID].componentIndices.end())
            {
                ID componentIndex = objects[objectID].componentIndices[C::componentFamily()];

                //Remove the component from its array
                getComponentArray<C>().remove(componentIndex);

                //Remove the pointer to the component maps from the object
                objects[objectID].componentArrays.erase(C::componentFamily());
                objects[objectID].componentIndices.erase(C::componentFamily());

                totalRemoved = 1;
            }

            return totalRemoved + remove<Args...>(objectID);
        }
    }

    return totalRemoved;
}

/** \brief Associate a string with a component
 *
 * \param compName The string to register
 */
template<typename C>
void ObjectManager::bindStringToComponent(const std::string& compName)
{
    //Only bind the string if it is not already in use.
    if (stringToCompFamily.find(compName) == stringToCompFamily.end())
        stringToCompFamily[compName] = C::componentFamily();
}

//!Must be called before using components
template<typename C, typename ... Args>
void ObjectManager::registerComponents()
{
    registerComponent<C>();
    if (sizeof...(Args) > 0)
        registerComponents<Args...>();
}

//!Called on component creation
template<typename C>
void ObjectManager::registerComponent()
{
    if (compFamilyToCompArray.find(C::componentFamily()) == compFamilyToCompArray.end())
    {
        compFamilyToProtoCompArray[C::componentFamily()] = &getPrototypeComponentArray<C>();
        compFamilyToCompArray[C::componentFamily()] = &getComponentArray<C>();

        //Bind component to it's name if set
        if (C::componentName() != "")
            bindStringToComponent<C>(C::componentName());
    }
}

/** \brief Get the total number of the specified component.
 *
 * \return The total number of the specified component.
 *
 */
template<typename C>
ID ObjectManager::getTotalComponents() const
{
    return getComponentArray<C>().size();
}

/** \brief Get a pointer to the specified component that belongs to the object with the given id.
 *
 *         WARNING: Pointer will be invalidated if the component array is resized
 *         Do not store the pointer, and do not add more components of the same
 *         type before using the pointer
 *
 *         If the object does not have the specified component, the returned pointer
 *         will be null. Please check for this before using.
 *
 *         To keep track of an object, store the ID and call this function every update.
 *
 * \param objectID The owner object's id.
 * \return If the object has an instance of the specified component, returns a pointer to the component. If
 *         the object does not have an instance, return a nullptr.
 *
 */
template<typename C>
C* const ObjectManager::getComponent(ID objectID)
{
    C* compPtr = nullptr;

    if(objects.isValid(objectID))
    {
        //If the object has the specified component
        auto& compIndices = objects[objectID].componentIndices;
        auto found = compIndices.find(C::componentFamily());
        if(found != compIndices.end())
            compPtr = &getComponentArray<C>()[found->second];
    }
    return compPtr;
}

//!Check if a prototype has the specified prototype.
template<typename C, typename ... Args>
bool ObjectManager::hasComponentsPrototype(const std::string& prototypeName)
{
    //Get the prototype
    auto& prototype = objectPrototypes[prototypeName];

    if(!SentinalType::endRecursion(C()))
    {
        if(prototype.componentIndices.find(C::componentFamily()) != prototype.componentIndices.end())
            //Return a pointer to the component converted to a boolean
            return true && hasComponentsPrototype<Args...>(prototypeName);

        return false;
    }

    return true;
}

/** \brief Add components to the prototype that has the given name.
 *         If there is no existing prototype under the given name, one is created.
 *         A prototype may only have one instance of each type of component.
 *
 * \param prototypeName The name of the prototype to add the components to.
 * \param first The first component to add to the prototype.
 * \param others Any other components to add to the prototype.
 *
 */
template<typename C, typename ... Args, typename>
void ObjectManager::assignToPrototype(const std::string& prototypeName, const C& first, Args&& ... others)
{
    registerComponent<C>();

    //Get the prototype
    auto& prototype = objectPrototypes[prototypeName];

    if(prototype.objectID == ID(-1))
        prototype.objectID = prototypeIDCounter++;

    //If the prototype does not already have the given component
    if(prototype.componentArrays.find(C::componentFamily()) == prototype.componentArrays.end())
    {
        //Add the first component in the list
        auto compIdx = getPrototypeComponentArray<C>().addItem(first);

        prototype.componentArrays[C::componentFamily()] = &getPrototypeComponentArray<C>();
        prototype.componentIndices[C::componentFamily()] = compIdx;

        //If there are more components to add, keep adding them
        if(sizeof...(others) > 0)
            assignToPrototype(prototypeName, others...);
    }

}

template<typename ... Args>
void ObjectManager::assignToPrototype(const std::string& prototypeName, const std::string& compName, const std::string& compValues, Args&& ... others)
{
    //Get the prototype
    auto& prototype = objectPrototypes[prototypeName];
    if(prototype.objectID == ID(-1))
        prototype.objectID = prototypeIDCounter++;

    assignFromString(prototype, compFamilyToProtoCompArray, compName, compValues, others...);
}

template<typename ... Args>
ID ObjectManager::assignFromString(Object& obj, std::unordered_map<ID, BaseComponentArray*>& compFamilyArray, const std::string& compName, const std::string& compValues, Args&& ... others)
{
    ID numAdded = 0;
    if(stringToCompFamily.find(compName) != stringToCompFamily.end())
    {
        auto componentFamily = stringToCompFamily[compName];
        //std::cout << "Adding component family: " << componentFamily << std::endl;

        //If the object does not already have the given component, add it
        if(obj.componentArrays.find(componentFamily) == obj.componentArrays.end())
        {
            //Add the first component in the list
            auto compArray = compFamilyArray[componentFamily];
            auto compIdx = compArray->addItem(compValues);

            //Store pointers to the component arrays and index in the object
            obj.componentArrays[componentFamily] = compArray;
            obj.componentIndices[componentFamily] = compIdx;

            //Set the new component's owner id
            obj.componentArrays[componentFamily]->getBaseComponent(compIdx).ownerID = obj.objectID;
            ++numAdded;
        }
        //If the object already has an instance, get a pointer to the object's component and deserialize the string
        else
            obj.componentArrays[componentFamily]->getBaseComponent(obj.componentIndices[componentFamily]).deSerialize(compValues);
    }
    else
        std::cerr << "Error: " << compName << " not bound to a component\n";
    //std::cout << "Finished Adding component family: " << componentFamily << std::endl;

    //If there are more components to add, keep adding them
    if(sizeof...(others) > 0)
        return numAdded + assignFromString(obj, compFamilyArray, others...);
    return numAdded;
}

/** \brief Remove the specified component from the prototype with the given name.
 *
 * \param prototypeName The name of the prototype to remove the component from.
 */
template<typename C>
void ObjectManager::removeFromPrototype(const std::string& prototypeName)
{
    //If the prototype has the specified component
    if(objectPrototypes.find(prototypeName) != objectPrototypes.end())
    {
        auto& compArray = objectPrototypes[prototypeName].componentArrays;
        auto& compIndices = objectPrototypes[prototypeName].componentIndices;

        //If the prototype has the component to remove
        if(compArray.find(C::componentFamily()) != compArray.end())
        {
            compArray[C::componentFamily()]->remove(compIndices[C::componentFamily()]);

            compArray.erase(C::componentFamily());
            compIndices.erase(C::componentFamily());
        }
    }
}

template<typename C, typename ... Args>
bool ObjectManager::hasComponents(ID objectID)
{
    //Get rid of instantiation here and change endRecursion to only use template params
    if(!SentinalType::endRecursion(C()))
    {
        if(getComponent<C>(objectID))
        {
            //Return a pointer to the component converted to a boolean
            return true && hasComponents<Args...>(objectID);
        }
        return false;
    }

    return true;
}

}//ocs

#endif

