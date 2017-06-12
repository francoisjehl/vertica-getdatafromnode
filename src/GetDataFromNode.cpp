#include "Vertica.h"
#include <sstream>

#define NODE_PARAM "node"
#define CANCEL_CHECK_GRAIN 10000

using namespace Vertica;

class GetDataFromNode : public TransformFunction
{
    private:
        std::string* _node;

    public:
        virtual void setup(ServerInterface &srvInterface,
                           const SizedColumnTypes &argTypes)
        {
            ParamReader paramReader = srvInterface.getParamReader();
            if ( paramReader.containsParameter(NODE_PARAM) )
                _node = new std::string(paramReader.getStringRef(NODE_PARAM).data());  
            else
                vt_report_error(0, "Parameter %s was not found in ParamReader", NODE_PARAM);    
        }
   
        virtual void processPartition(ServerInterface &srvInterface, 
                                      PartitionReader &inputReader, 
                                      PartitionWriter &outputWriter)
        {
            if ( srvInterface.getCurrentNodeName() == *_node )
	    {
	        try
	        {
                    // Get columns passed as arguments (usually *), and returns a vector of their indexes
                    const SizedColumnTypes &inTypes = inputReader.getTypeMetaData();
                    std::vector<size_t> argCols; 
                    inTypes.getArgumentColumns(argCols);
                    do 
                    {
                        // Write the input to the output
                        size_t columnIndex = 0;
                        for (std::vector<size_t>::iterator i = argCols.begin(); i < argCols.end(); i++)
                            outputWriter.copyFromInput(columnIndex++, inputReader, *i);
                        outputWriter.next();
                    } 
                    while (inputReader.next());
                }
                catch(std::exception& e) 
                {
                    // Standard exception. Quit.
                    vt_report_error(0, "Exception while processing partition: [%s]", e.what());
                }
            }
        }

        virtual void destroy(ServerInterface &srvInterface, 
                             const SizedColumnTypes &argTypes)
        {
            delete _node;
        }
};


class GetDataFromNodeFactory : public TransformFunctionFactory
{
    virtual void getPrototype(ServerInterface &srvInterface, ColumnTypes &argTypes, ColumnTypes &returnType)
    {
        argTypes.addAny();
        returnType.addAny();
    }

    virtual void getReturnType(ServerInterface &srvInterface, 
                               const SizedColumnTypes &inputTypes, 
                               SizedColumnTypes &outputTypes)
    {
        // The return type is just precisely the input: we output what has been supplied
        std::vector<size_t> argCols;
        inputTypes.getArgumentColumns(argCols);        
        for (std::vector<size_t>::iterator i = argCols.begin(); i < argCols.end(); i++)
        {
            outputTypes.addArg(inputTypes.getColumnType(*i), inputTypes.getColumnName(*i));
        }
    }
    
    virtual void getParameterType(ServerInterface &srvInterface,
                                  SizedColumnTypes &parameterTypes)
    {
        parameterTypes.addVarchar(255, NODE_PARAM);
    }
    
    virtual TransformFunction *createTransformFunction(ServerInterface &srvInterface)
    { 
      return vt_createFuncObject<GetDataFromNode>(srvInterface.allocator);
    }

};
// Registers the factory
RegisterFactory(GetDataFromNodeFactory);

// Adding metadata to the libary
RegisterLibrary(
    "François Jehl",
    "1",
    "0.1",
    "7.2.3-18",
    "http://www.github.com/francoisjehl/vertica-getdatafromnode",
    "UDTF that output data hosted by the supplied node name",
    "",
    ""
);

