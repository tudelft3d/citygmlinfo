
#include <tclap/CmdLine.h>
#include <time.h>  
#include <fstream>
#include <string>
#include "pugixml.hpp"


void        parsefileandreport(std::string& ifile);
std::string localise(std::string s);
void        report_building(pugi::xml_document& doc);
void        report_primitives(pugi::xml_document& doc);


class MyOutput : public TCLAP::StdOutput
{
public:
  
  virtual void usage(TCLAP::CmdLineInterface& c)
  {
    std::cout << "===== val3dity =====" << std::endl;
    std::cout << "OPTIONS" << std::endl;
    std::list<TCLAP::Arg*> args = c.getArgList();
    for (TCLAP::ArgListIterator it = args.begin(); it != args.end(); it++) {
      if ((*it)->getFlag() == "")
        std::cout << "\t--" << (*it)->getName() << std::endl;
      else
        std::cout << "\t-" << (*it)->getFlag() << ", --" << (*it)->getName() << std::endl;
      std::cout << "\t\t" << (*it)->getDescription() << std::endl;
    }
    std::cout << "EXAMPLES" << std::endl;
    std::cout << "\tval3dity input.gml" << std::endl;
    std::cout << "\t\tValidates each gml:Solid in input.gml and outputs a summary" << std::endl;
    std::cout << "\tval3dity input.obj" << std::endl;
    std::cout << "\t\tValidates each object in the OBJ file and outputs a summary" << std::endl;
    std::cout << "\tval3dity input.gml -p MS" << std::endl;
    std::cout << "\t\tValidates each gml:MultiSurface in input.gml and outputs a summary" << std::endl;
    std::cout << "\tval3dity input.gml --oxml report.xml" << std::endl;
    std::cout << "\t\tValidates each gml:Solid in input.gml and outputs a detailed report in XML" << std::endl;
    std::cout << "\tval3dity data/poly/cube.poly --ishell data/poly/a.poly" << std::endl;
    std::cout << "\t\tValidates the solid formed by the outer shell cube.poly with the inner shell a.poly" << std::endl;
    std::cout << "\tval3dity input.gml --verbose" << std::endl;
    std::cout << "\t\tAll details of the validation of the solids is printed out" << std::endl;
    std::cout << "\tval3dity input.gml --snap_tolerance 0.1" << std::endl;
    std::cout << "\t\tThe vertices in gml:Solid closer than 0.1unit are snapped together" << std::endl;
    std::cout << "\tval3dity input.gml --planarity_d2p 0.1" << std::endl;
    std::cout << "\t\tValidates each gml:Solid in input.gml" << std::endl;
    std::cout << "\t\tand uses a tolerance of 0.1unit (distance point-to-fitted-plane)" << std::endl;
  }
};


int main(int argc, char* const argv[])
{
  std::streambuf* savedBufferCLOG;
  std::ofstream mylog;

  //-- tclap options
  std::vector<std::string> primitivestovalidate;
  primitivestovalidate.push_back("S");  
  primitivestovalidate.push_back("CS");   
  primitivestovalidate.push_back("MS");   
  TCLAP::ValuesConstraint<std::string> primVals(primitivestovalidate);

  TCLAP::CmdLine cmd("Allowed options", ' ', "1.1");
  MyOutput my;
  cmd.setOutput(&my);
  try {
    TCLAP::UnlabeledValueArg<std::string>  inputfile("inputfile", "input file in either GML (several gml:Solids possible) or POLY (one exterior shell)", true, "", "string");
    TCLAP::SwitchArg                       buildings("B", "Buildings", "report uses the CityGML Buildings", false);
    TCLAP::SwitchArg                       verbose("", "verbose", "verbose output", false);
    TCLAP::SwitchArg                       unittests("", "unittests", "unit tests output", false);
    TCLAP::SwitchArg                       onlyinvalid("", "onlyinvalid", "only invalid primitives are reported", false);
    TCLAP::SwitchArg                       qie("", "qie", "use the OGC QIE error codes", false);
    TCLAP::ValueArg<double>                snap_tolerance("", "snap_tolerance", "tolerance for snapping vertices in GML (default=0.001)", false, 0.001, "double");
    TCLAP::ValueArg<double>                planarity_d2p("", "planarity_d2p", "tolerance for planarity distance_to_plane (default=0.01)", false, 0.01, "double");
    TCLAP::ValueArg<double>                planarity_n("", "planarity_n", "tolerance for planarity based on normals deviation (default=1.0degree)", false, 1.0, "double");

    cmd.add(qie);
    cmd.add(planarity_d2p);
    cmd.add(planarity_n);
    cmd.add(snap_tolerance);
    cmd.add(buildings);
    cmd.add(verbose);
    cmd.add(unittests);
    cmd.add(onlyinvalid);
    cmd.add(inputfile);
    cmd.parse( argc, argv );

    parsefileandreport(inputfile.getValue());
    
    return(1);
  }
  catch (TCLAP::ArgException &e) {
    std::cout << "ERROR: " << e.error() << " for arg " << e.argId() << std::endl;
    return(0);
  }
}

void report_primitives(pugi::xml_document& doc) {
  std::cout << "+++++++++++++++++++ PRIMITIVES +++++++++++++++++++" << std::endl;
  
  std::string s = "//" + localise("Solid");
  pugi::xpath_node_set tmp = doc.select_nodes(s.c_str());
  std::cout << "# of <gml:Solid>: " << tmp.size() << std::endl;

  s = "//" + localise("MultiSolid");
  tmp = doc.select_nodes(s.c_str());
  std::cout << "# of <gml:MultiSolid>: " << tmp.size() << std::endl;

  s = "//" + localise("MultiSurface");
  tmp = doc.select_nodes(s.c_str());
  std::cout << "# of <gml:MultiSurface>: " << tmp.size() << std::endl;

  s = "//" + localise("CompositeSurface");
  tmp = doc.select_nodes(s.c_str());
  std::cout << "# of <gml:CompositeSurface>: " << tmp.size() << std::endl;

  s = "//" + localise("CompositeSolid");
  tmp = doc.select_nodes(s.c_str());
  std::cout << "# of <gml:CompositeSolid>: " << tmp.size() << std::endl;
}

void report_building(pugi::xml_document& doc) {
  std::cout << "+++++++++++++++++++ BUILDINGS +++++++++++++++++++" << std::endl;
  
  std::string s = "//" + localise("Building");
  pugi::xpath_node_set tmp = doc.select_nodes(s.c_str());
  int nobuildings = tmp.size();
  std::cout << "# of Building: " << nobuildings << std::endl;

  s = "//" + localise("Building") + "[@" + localise("id") + "]";
  tmp = doc.select_nodes(s.c_str());
  std::cout << "# of Building with 'gml:id': " << tmp.size() << std::endl;

  s = "//" + localise("BuildingPart");
  tmp = doc.select_nodes(s.c_str());
  std::cout << "# of BuildingPart: " << tmp.size() << std::endl;

  s = "//" + localise("BuildingPart") + "[@" + localise("id") + "]";
  tmp = doc.select_nodes(s.c_str());
  std::cout << "# of BuildingPart with 'gml:id': " << tmp.size() << std::endl;

  s = "//" + localise("Building");
  pugi::xpath_node_set nbuildings = doc.select_nodes(s.c_str());
  int counter = 0;
  for (auto& nb: nbuildings) {
    std::string s1 = ".//" + localise("BuildingPart");
    pugi::xpath_node_set nbps = nb.node().select_nodes(s1.c_str());
    if (nbps.empty() == false) {
      for (auto& nbp : nbps) {
        counter++;
        break;
      }
    }
  }
  std::cout << "# of Building splitted into Part: " << counter << std::endl;

  s = "//" + localise("Building") + "//" + localise("Solid");
  tmp = doc.select_nodes(s.c_str());
  if (tmp.size() == 0)
    std::cout << "Buildings stored with MS" << std::endl;
  else
    std::cout << "Buildings stored with <gml:Solid>" << std::endl;


}


void parsefileandreport(std::string& ifile) {
  std::cout << "Reading file: " << ifile << std::endl;
  pugi::xml_document doc;
  if (!doc.load_file(ifile.c_str())) 
  {
    std::cerr << "File not found" << std::endl;
    return;
  }

  report_building(doc);

  report_primitives(doc);

}

//-- ignore XML namespace
std::string localise(std::string s) {
  return "*[local-name(.) = '" + s + "']";
}
