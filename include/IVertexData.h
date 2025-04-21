#ifndef IVERTEXDATA_H_
#define IVERTEXDATA_H_

#include <string>
#include <vector>
#include <stdexcept>
using namespace std;

namespace util
{

/*
 * Interface for any class that stores and processes vertex attributes
 * Each attribute will have a unique name.
 */
class IVertexData
{
public:
    /*
     * Query if attribute by the given name is present
     * \param attribName the name of the attribute that is being queried
     * \return true if data for this name is present, false otherwise
     */
    virtual bool hasData(string attribName)=0;
    /*
     * Returns the data for the supplied attribute name as a float array, for
     * maximum flexibility
     * \param attribName the (unique) name of the attribute
     * \return the attribute data as a float array
     */
    virtual vector<float> getData(string attribName) =0;

    /*
     * set the data for the given attribute. If attribute is not already present,
     * it will be added now. If data is already present for the attribute,
     * it will be overwritten
     * \param attribName the name of the attribute to be added/set
     * \param data the data as an array of floats for maximum flexibility
     * \throws runtime_error if an implementation does not support that attribute
     */
    virtual void setData(string attribName,
                         const vector<float>& data)=0;

    /*
     * Get a list of all the attribute names supported by an implementation
     * \return an array of String objects storing the name of all supported
     *         attribute names
     */
    virtual vector<string> getAllAttributes()=0;
};
}

#endif
