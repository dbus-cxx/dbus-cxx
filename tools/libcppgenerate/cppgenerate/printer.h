#include <vector>
#include <string>
#include <ostream>

#include "class.h"

namespace cppgenerate{

/**
 * The Printer class handles all of the needed functions in 
 * order to actually generate the code.  Once you have the
 * definitions of classes/enums/etc, add them to the printer,
 * which will then go and generate the otuput.
 */
class Printer {
  public:
    Printer();

    Printer( const Printer& other );

    Printer& addClass( const Class& cls );

    Printer& generateAsSingleFile( bool singleFile );

    Printer& setOutputDirectory( std::string outputDirectory );

    Printer& print();

  private:
    void printClass( const Class& cls, std::ostream ostream );

  private:
    std::vector<Class> m_classes;
    bool m_generateAsSingleFile;
    std::string m_outputDirectory;
};

}
