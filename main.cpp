
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
  std::cout << std::setw(35) << std::left  << "<gml:Solid>";
  std::cout << std::setw(15) << std::right << doc.select_nodes(s.c_str()).size() << std::endl;

  s = "//" + localise("MultiSolid");
  std::cout << std::setw(35) << std::left  << "<gml:MultiSolid>";
  std::cout << std::setw(15) << std::right << doc.select_nodes(s.c_str()).size() << std::endl;

  s = "//" + localise("CompositeSolid");
  std::cout << std::setw(35) << std::left  << "<gml:CompositeSolid>";
  std::cout << std::setw(15) << std::right << doc.select_nodes(s.c_str()).size() << std::endl;
  
  s = "//" + localise("MultiSurface");
  std::cout << std::setw(35) << std::left  << "<gml:MultiSurface>";
  std::cout << std::setw(15) << std::right << doc.select_nodes(s.c_str()).size() << std::endl;
  
  s = "//" + localise("CompositeSurface");
  std::cout << std::setw(35) << std::left  << "<gml:CompositeSurface>";
  std::cout << std::setw(15) << std::right << doc.select_nodes(s.c_str()).size() << std::endl;

  std::cout << std::endl;
}


void report_building(pugi::xml_document& doc) {
  std::cout << "+++++++++++++++++++ BUILDINGS +++++++++++++++++++" << std::endl;
  std::string s;
  s = "//" + localise("Building");
  std::cout << std::setw(35) << std::left  << "<Building>";
  int nobuildings = doc.select_nodes(s.c_str()).size();
  std::cout << std::setw(15) << std::right << nobuildings << std::endl;
  s = "//" + localise("Building") + "[@" + localise("id") + "]";
  if (doc.select_nodes(s.c_str()).size() == nobuildings)
    std::cout << "(all of them have gml:id)" << std::endl;
  else if (doc.select_nodes(s.c_str()).size() == 0)
    std::cout << "(none of them have gml:id)" << std::endl;
  else
    std::cout << "(some of them have gml:id, but not all)" << std::endl;


  s = "//" + localise("BuildingPart");
  std::cout << std::setw(35) << std::left  << "<BuildingPart>";
  int nobuildingparts = doc.select_nodes(s.c_str()).size();
  std::cout << std::setw(15) << std::right << nobuildingparts << std::endl;
  s = "//" + localise("BuildingPart") + "[@" + localise("id") + "]";
  if (doc.select_nodes(s.c_str()).size() == nobuildingparts)
    std::cout << "(all of them have gml:id)" << std::endl;
  else if (doc.select_nodes(s.c_str()).size() == 0)
    std::cout << "(none of them have gml:id)" << std::endl;
  else
    std::cout << "(some of them have gml:id, but not all)" << std::endl;

  s = "//" + localise("Building");
  pugi::xpath_node_set nbuildings = doc.select_nodes(s.c_str());
  int c1 = 0;
  std::string s1 = ".//" + localise("BuildingPart");
  std::string s2 = ".//" + localise("Solid");
  for (auto& nb: nbuildings) {
    //-- BuildingPart
    pugi::xpath_node_set nbps = nb.node().select_nodes(s1.c_str());
    if (nbps.empty() == false) {
      for (auto& nbp : nbps) {
        c1++;
        break;
      }
    }
    //-- primitives
    // pugi::xpath_node_set tmp = nb.node().select_nodes(s2.c_str());
    // if (tmp.empty() == false) {
    //   for (auto& nbp : tmp) {
    //     c2++;
    //     break;
    //   }
    // }
  }
  std::cout << std::setw(35) << std::left  << "<Building> having <BuildingPart>";
  std::cout << std::setw(15) << std::right << c1 << std::endl;


  s = "//" + localise("Building") + "//" + localise("Solid");
  if (doc.select_nodes(s.c_str()).size() == 0)
    std::cout << "Buildings stored with <gml:MultiSurface>" << std::endl;
  else
    std::cout << "Buildings stored with <gml:Solid>" << std::endl;

  std::cout << std::endl;
}


void parsefileandreport(std::string& ifile) {
  std::cout << "Reading file: " << ifile << std::endl << std::endl;
  pugi::xml_document doc;
  if (!doc.load_file(ifile.c_str())) 
  {
    std::cerr << "File not found" << std::endl;
    return;
  }

  //-- A//B/*[1]
  // std::string s = "//" + localise("Building") + "//" + localise("BuildingPart") + "[1]";
  // std::string s = "(//" + localise("Building") + "//" + localise("BuildingPart") + ")[1]";
  // std::string s = "//" + localise("Building") + "//" + localise("BuildingPart");
  // std::cout << s << std::endl;
  // pugi::xpath_node_set tmp = doc.select_nodes(s.c_str());
  // std::cout << "# : " << tmp.size() << std::endl;

  // std::string s = "count(//" + localise("Solid") + ")";
  // pugi::xpath_node tmp = doc.select_node(s.c_str());
  // std::cout << tmp.attribute() << std::endl;

  report_primitives(doc);
  report_building(doc);

}

//-- ignore XML namespace
std::string localise(std::string s) {
  return "*[local-name(.) = '" + s + "']";
}
// std::string localise(std::string s) {
//   return ".[local-name() = '" + s + "']";
// }
