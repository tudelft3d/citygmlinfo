
#include <tclap/CmdLine.h>
#include <time.h>  
#include <fstream>
#include <string>
#include "pugixml.hpp"
#include "boost/locale.hpp"


std::string localise(std::string s);
void        report_building(pugi::xml_document& doc);
void        report_primitives(pugi::xml_document& doc);
void        print_info_aligned(std::string o, size_t number);



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

  boost::locale::generator gen;
  std::locale loc = gen("en_US.UTF-8");
  std::locale::global(loc);
  std::cout.imbue(loc);
  

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
    TCLAP::SwitchArg                       buildings("B", "Buildings", "info about the Buildings", false);
    TCLAP::SwitchArg                       geomprimitives("P", "geomprimitives", "unit tests output", false);
    TCLAP::SwitchArg                       verbose("", "verbose", "verbose output", false);

    cmd.add(buildings);
    cmd.add(geomprimitives);
    cmd.add(verbose);
    cmd.add(inputfile);
    cmd.parse( argc, argv );

    std::cout << "Reading file: " << inputfile.getValue() << std::endl << std::endl;
    pugi::xml_document doc;
    if (!doc.load_file(inputfile.getValue().c_str())) 
    {
      std::cerr << "File not found" << std::endl;
      return 0;
    }

    // std::string s = "//" + localise("Building") + "/" + localise("lod2Solid") + "[1]";
    // int total = doc.select_nodes(s.c_str()).size();
    // s = "//" + localise("Building") + "/" + localise("consistsOfBuildingPart") + "/" + localise("BuildingPart") + "/" + localise("lod2Solid") + "[1]";
    // std::cout << doc.select_nodes(s.c_str()).size() << std::endl;
    // total += doc.select_nodes(s.c_str()).size();
    // std::cout << total << std::endl;
    

    report_primitives(doc);
    report_building(doc);
    
    return 1;
  }
  catch (TCLAP::ArgException &e) {
    std::cout << "ERROR: " << e.error() << " for arg " << e.argId() << std::endl;
    return(0);
  }
}

void print_info_aligned(std::string o, size_t number) {
  std::cout << std::setw(35) << std::left  << o;
  std::cout << std::setw(15) << std::right << boost::locale::as::number << number << std::endl;
}

void report_primitives(pugi::xml_document& doc) {
  std::cout << "+++++++++++++++++++ PRIMITIVES +++++++++++++++++++" << std::endl;
  
  std::string s = "//" + localise("Solid");
  print_info_aligned("gml:Solid", doc.select_nodes(s.c_str()).size());

  s = "//" + localise("MultiSolid");
  print_info_aligned("gml:MultiSolid", doc.select_nodes(s.c_str()).size());

  s = "//" + localise("CompositeSolid");
  print_info_aligned("gml:CompositeSolid", doc.select_nodes(s.c_str()).size());
  
  s = "//" + localise("MultiSurface");
  print_info_aligned("gml:MultiSurface", doc.select_nodes(s.c_str()).size());
  
  s = "//" + localise("CompositeSurface");
  print_info_aligned("gml:CompositeSurface", doc.select_nodes(s.c_str()).size());

  s = "//" + localise("Polygon");
  print_info_aligned("gml:Polygon", doc.select_nodes(s.c_str()).size());

  std::cout << std::endl;
}


void report_building(pugi::xml_document& doc) {
  std::cout << "++++++++++++++++++++ BUILDINGS +++++++++++++++++++" << std::endl;
  
  std::string s = "//" + localise("Building");
  int nobuildings = doc.select_nodes(s.c_str()).size();
  print_info_aligned("Building", nobuildings);

  s = "//" + localise("Building") + "[@" + localise("id") + "]";
  if (doc.select_nodes(s.c_str()).size() == nobuildings)
    std::cout << "(all of them have gml:id)" << std::endl;
  else if (doc.select_nodes(s.c_str()).size() == 0)
    std::cout << "(none of them have gml:id)" << std::endl;
  else
    std::cout << "(some of them have gml:id, but not all)" << std::endl;

  s = "//" + localise("Building") + "/" + localise("consistsOfBuildingPart") + "[1]";
  int nobwbp = doc.select_nodes(s.c_str()).size();
  print_info_aligned("Building having BuildingPart", nobwbp);
  print_info_aligned("Building without BuildingPart", (nobuildings - nobwbp));


  s = "//" + localise("BuildingPart");
  int nobuildingparts = doc.select_nodes(s.c_str()).size();
  print_info_aligned("BuildingPart", nobuildingparts);

  // s = "//" + localise("BuildingPart") + "[@" + localise("id") + "]";
  // if (doc.select_nodes(s.c_str()).size() == nobuildingparts)
  //   std::cout << "(all of them have gml:id)" << std::endl;
  // else if (doc.select_nodes(s.c_str()).size() == 0)
  //   std::cout << "(none of them have gml:id)" << std::endl;
  // else
  //   std::cout << "(some of them have gml:id, but not all)" << std::endl;


  s = "//" + localise("Building") + "/" + localise("lod1Solid") + "[1]";
  print_info_aligned("Building with LOD1 gml:Solid", doc.select_nodes(s.c_str()).size());
  s = "//" + localise("Building") + "/" + localise("consistsOfBuildingPart") + "/" + localise("BuildingPart") + "/" + localise("lod1Solid") + "[1]";
  print_info_aligned("BuildingPart with LOD1 gml:Solid", doc.select_nodes(s.c_str()).size());
  
  s = "//" + localise("Building") + "/" + localise("lod2Solid") + "[1]";
  print_info_aligned("Building with LOD2 gml:Solid", doc.select_nodes(s.c_str()).size());
  s = "//" + localise("Building") + "/" + localise("consistsOfBuildingPart") + "/" + localise("BuildingPart") + "/" + localise("lod2Solid") + "[1]";
  print_info_aligned("BuildingPart with LOD2 gml:Solid", doc.select_nodes(s.c_str()).size());

  s = "//" + localise("Building") + "/" + localise("lod3Solid") + "[1]";
  print_info_aligned("Building with LOD3 gml:Solid", doc.select_nodes(s.c_str()).size());
  s = "//" + localise("Building") + "/" + localise("consistsOfBuildingPart") + "/" + localise("BuildingPart") + "/" + localise("lod3Solid") + "[1]";
  print_info_aligned("BuildingPart with LOD3 gml:Solid", doc.select_nodes(s.c_str()).size());
  

  // s = "//" + localise("Building") + "//" + localise("Solid");
  // if (doc.select_nodes(s.c_str()).size() == 0)
  //   std::cout << "Buildings stored with <gml:MultiSurface>" << std::endl;
  // else
  //   std::cout << "Buildings stored with <gml:Solid>" << std::endl;


  // int c1 = 0;
  // std::string s1 = ".//" + localise("BuildingPart");
  // std::string s2 = ".//" + localise("Solid");
  // for (auto& nb: nbuildings) {
  //   //-- BuildingPart
  //   pugi::xpath_node_set nbps = nb.node().select_nodes(s1.c_str());
  //   if (nbps.empty() == false) {
  //     for (auto& nbp : nbps) {
  //       c1++;
  //       break;
  //     }
  //   }
  //   //-- primitives
  //   // pugi::xpath_node_set tmp = nb.node().select_nodes(s2.c_str());
  //   // if (tmp.empty() == false) {
  //   //   for (auto& nbp : tmp) {
  //   //     c2++;
  //   //     break;
  //   //   }
  //   // }
  // }
  // std::cout << std::setw(35) << std::left  << "<Building> having <BuildingPart>";
  // std::cout << std::setw(15) << std::right << c1 << std::endl;



  std::cout << std::endl;
}


//-- ignore XML namespace
std::string localise(std::string s) {
  return "*[local-name(.) = '" + s + "']";
}
// std::string localise(std::string s) {
//   return ".[local-name() = '" + s + "']";
// }
