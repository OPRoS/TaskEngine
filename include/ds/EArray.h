/** 
* @file XabslArray.h
*
* Declaration and implementation of template class Array
*
* @author <a href="http://www.martin-loetzsch.de">Martin Loetzsch</a>
* @author <a href="http://www.informatik.hu-berlin.de/virtual ~juengel">Matthias JŸngel</a>
* @author <a href="http://www.tzi.de/virtual ~roefer/">Thomas Ršfer</a>
*/


#ifndef __EArray_h_
#define __EArray_h_

#include <stdlib.h>
#include <string.h>

class NamedItem
{
private:
	char* n;

public:
	NamedItem(const char* name){
		n = new char[strlen(name)+1];
		strcpy(n,name);
	}

	virtual ~NamedItem() { 
		delete[] n; 
	}
};


template<class T> class ArrayElement : public NamedItem
{
private:
	T e;

public:
	ArrayElement(const char* name, T element)
	: NamedItem(name), e(element) {}

	virtual ~ArrayElement() {}
};

template <class T> class Array
{

protected:
    /** 
    * Returns the index of an element with the given name.
    * @param name The name that is searched for.
    * @return The index of the element of -1 if the name does not exist.
    */
    int find(const char* name) const
    {
      for(unsigned int i = 0; i < getSize(); ++i)
        if(!strcmp(getName(i),name)) 
          return i;
      return -1;
    }

private:
    /** The array */
    ArrayElement<T>** data;

    /** The number of elements in the array */
    int usedSize, allocatedSize;

public:
    /** Constructor */
    Array() 
    {
      usedSize = 0;
      allocatedSize = 2;
      data = new ArrayElement<T>*[allocatedSize];
    }
    
    /** Destructor */
    virtual ~Array() 
    { 
      for(unsigned int i = 0; i < getSize(); ++i)
        delete data[i];
      delete[] data;
    }

    /** Clears the array */
    void clear()
    {
      for(unsigned int i = 0; i < getSize(); ++i)
        delete data[i];
      delete[] data;
      usedSize = 0;
      allocatedSize = 2;
      data = new ArrayElement<T>*[allocatedSize];
    }
    
    /** 
    * Returns the value for a given name. 
    * If no element exists for the name, the default value is returned.
    * @param name The name element
    * @param defaultValue The value that is returned if no element exists for the name.
    * @return Either the element found or the default value. 
    */
    T getElement(const char* name, T defaultValue) const
    {
      int pos = find(name);
      if(pos < 0) 
        return defaultValue;
      else
        return getElement(pos);
    }
    
    /** 
    * Returns the value for a given name. 
    * Note that the function crashes if the element does not exist.
    * @param name The name of the element
    */
    T& getElement(const char* name) const
    {
      return getElement(find(name));
    }

    /** 
    * Returns the value for a given array position.
    * Note that the function crashes if the required position is bigger than the 
    * size of the array.
    */
    T& getElement(int pos) const
    {
      return data[pos]->e;
    }
    
    /** 
    * Returns a pointer to the array element for a given name.
    * Note that the function crashes if the element does not exist
    * @param name the name of the element 
    */
    ArrayElement<T>* getPElement(const char* name)
    {
      return data[find(name)];
    }

    /** Returns the name of an element */
    const char* getName(int pos) const
    { 
      return data[pos]->n;
    }

    /**
    * The function appends a new element to the array.
    * @param name A string label for the element.
    * @param element The new element.
    */
    void append(const char* name, T element)
    {
      if(usedSize == allocatedSize)
      {
        allocatedSize += allocatedSize / 2; // note that allocatedSize must be at least 2
        ArrayElement<T>** temp = new ArrayElement<T>*[allocatedSize];
        for(unsigned int i = 0; i < getSize(); ++i)
          temp[i] = data[i];
        delete[] data;
        data = temp;
      }
      data[usedSize++] = new ArrayElement<T>(name,element);
    }
    
    /**
    * The function sets the value of an element in the array.
    * Note that the function crashes if the element does not exist.
    * @param name A string label for the element.
    * @param value The new element.
    */
    void setElement(const char* name, T value)
    {
      setElement(find(name),value);
    }

    /**
    * The function sets the value of an element in the array.
    * Note that the function crashes if the element does not exist.
    * @param pos The position of the element in the array.
    * @param value The new element.
    */
    void setElement(int pos, T value)
    {
      data[pos]->e = value;
    }

    /**
    * The function returns the number of elements in the array.
    * @return The length of the list.
    */
    int getSize() const {return usedSize;}
    
    /** 
    * Returns the value for a given array position.
    * Note that the function crashes if the required position is bigger than the 
    * size of the array.
    */
    T operator[](int pos) const
    {
      return getElement(pos);
    }
    
    /** 
    * Returns the value for a given name. 
    * Note that the function crashes if the element does not exist.
    * @param name The name of the element
    */
    T operator[](const char* name) const
    {
      return getElement(name);
    }

    /** Returns whether an element for the given name exists */
    bool exists(const char* name) const
    {
      return find(name) >= 0;
    }

    /** Removes the last element of the array */
    void removeLast()
    {
      if (usedSize > 0) usedSize--;
    }

    operator Array<const T>&() {return this;}

 
};

#endif // __EArray_h_






