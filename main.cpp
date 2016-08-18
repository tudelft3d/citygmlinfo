
#include <tclap/CmdLine.h>
#include <map>  
#include <string>  
#include <time.h>  
#include <fstream>
#include <string>
#include "pugixml.hpp"
#include "boost/locale.hpp"


std::string localise(std::string s);
void        report_building(pugi::xml_document& doc, std::map<std::string, std::string>& ns);
void        report_primitives(pugi::xml_document& doc, std::map<std::string, std::string>& ns);
void        print_info_aligned(std::string o, size_t number, bool tab = false);
void        get_namespaces(pugi::xml_node& root, std::map<std::string, std::string>& ns);



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

  //-- XML namespaces map
  std::map<std::string, std::string> ns;
  
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

    std::cout << "Reading file: " << inputfile.getValue() << "... " << std::flush;
    pugi::xml_document doc;
    if (!doc.load_file(inputfile.getValue().c_str())) 
    {
      std::cerr << "File not found" << std::endl;
      return 0;
    }
    std::cout << "done." << std::endl << std::endl;

    //-- parse namespace
    pugi::xml_node ncm = doc.first_child();
    get_namespaces(ncm, ns);

    // // std::string s = "//" + ns["building"] + "Building" + "/" + ns["gml"] + "lod2Solid";
    // // std::cout << doc.select_nodes(s.c_str()).size() << std::endl;
    // // std::string s = "//" + ns["building"] + "Building[last()]";
    // std::string s = "//" + ns["building"] + "Building";
    // // pugi::xpath_node node = doc.select_node(s.c_str());
    // pugi::xpath_nodes nodes = doc.select_nodes(s.c_str());
    // // pugi::xpath_node_set nodes = doc.select_nodes(s.c_str())
    // for (auto& n : nodes)
    // {

    // }
    // for (pugi::xml_node tool = node.node().first_child(); tool; tool = tool.next_sibling())
    // {
    //   std::cout << tool.name() << std::endl;
    // }

    report_primitives(doc, ns);
    report_building(doc, ns);
    
    return 1;
  }
  catch (TCLAP::ArgException &e) {
    std::cout << "ERROR: " << e.error() << " for arg " << e.argId() << std::endl;
    return(0);
  }
}


void get_namespaces(pugi::xml_node& root, std::map<std::string, std::string>& ns) {
  for (pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()) {
    std::string name = attr.name();
    if (name.find("xmlns") != std::string::npos) {
      // std::cout << attr.name() << "=" << attr.value() << std::endl;
      std::string value = attr.value();
      std::string sns;
      if (value.find("http://www.opengis.net/citygml/0") != std::string::npos)
        sns = "citygml";
      else if (value.find("http://www.opengis.net/citygml/1") != std::string::npos)
        sns = "citygml";
      else if (value.find("http://www.opengis.net/citygml/2") != std::string::npos)
        sns = "citygml";
      else if (value.find("http://www.opengis.net/gml") != std::string::npos)
        sns = "gml";
      else if (value.find("http://www.opengis.net/citygml/building") != std::string::npos)
        sns = "building";
      else if (value.find("http://www.w3.org/1999/xlink") != std::string::npos)
        sns = "xlink";
      else
        sns = "";
      if (sns != "") {
        size_t pos = name.find(":");
        if (pos == std::string::npos) 
          ns[sns] = "";
        else 
          ns[sns] = name.substr(pos + 1) + ":";
      }    
    }
  }
}


void print_info_aligned(std::string o, size_t number, bool tab) {
  if (tab == false)
    std::cout << std::setw(40) << std::left  << o;
  else
    std::cout << "    " << std::setw(36) << std::left  << o;
  std::cout << std::setw(10) << std::right << boost::locale::as::number << number << std::endl;
}

void report_primitives(pugi::xml_document& doc, std::map<std::string, std::string>& ns) {
  std::cout << "+++++++++++++++++++ PRIMITIVES +++++++++++++++++++" << std::endl;
  
  std::string s = "//" + ns["gml"] + "Solid";
  print_info_aligned("gml:Solid", doc.select_nodes(s.c_str()).size());

  s = "//" + ns["gml"] + "MultiSolid";
  print_info_aligned("gml:MultiSolid", doc.select_nodes(s.c_str()).size());

  s = "//" + ns["gml"] + "CompositeSolid";
  print_info_aligned("gml:CompositeSolid", doc.select_nodes(s.c_str()).size());
  
  s = "//" + ns["gml"] + "MultiSurface";
  print_info_aligned("gml:MultiSurface", doc.select_nodes(s.c_str()).size());
  
  s = "//" + ns["gml"] + "CompositeSurface";
  print_info_aligned("gml:CompositeSurface", doc.select_nodes(s.c_str()).size());

  s = "//" + ns["gml"] + "Polygon";
  print_info_aligned("gml:Polygon", doc.select_nodes(s.c_str()).size());

  std::cout << std::endl;
}


void report_building(pugi::xml_document& doc, std::map<std::string, std::string>& ns) {
  std::cout << "++++++++++++++++++++ BUILDINGS +++++++++++++++++++" << std::endl;
  
  std::string s = "//" + ns["building"] + "Building";
  int nobuildings = doc.select_nodes(s.c_str()).size();
  print_info_aligned("Building", nobuildings);

  s = "//" + ns["building"] + "Building" + "/" + ns["building"] + "consistsOfBuildingPart" + "[1]";
  int nobwbp = doc.select_nodes(s.c_str()).size();
  print_info_aligned("without BuildingPart", (nobuildings - nobwbp), true);
  print_info_aligned("having BuildingPart", nobwbp, true);
  s = "//" + ns["building"] + "Building" + "[@" + ns["gml"] + "id]";
  print_info_aligned("with gml:id", doc.select_nodes(s.c_str()).size(), true);

  s = "//" + ns["building"] + "BuildingPart";
  int nobuildingparts = doc.select_nodes(s.c_str()).size();
  print_info_aligned("BuildingPart", nobuildingparts);
  s = "//" + ns["building"] + "BuildingPart" + "[@" + ns["gml"] + "id]";
  print_info_aligned("with gml:id", doc.select_nodes(s.c_str()).size(), true);
  
  std::cout << "LOD1" << std::endl;
  s = "//" + ns["building"] + "Building" + "/" + ns["building"] + "lod1Solid" + "[1]";
  print_info_aligned("Building stored in gml:Solid", doc.select_nodes(s.c_str()).size(), true);
  s = "//" + ns["building"] + "Building" + "/" + ns["building"] + "consistsOfBuildingPart" + "/" + ns["building"] + "BuildingPart" + "/" + ns["building"] + "lod1Solid" + "[1]";
  print_info_aligned("BuildingPart stored in gml:Solid", doc.select_nodes(s.c_str()).size(), true);
  
  std::cout << "LOD2" << std::endl;
  s = "//" + ns["building"] + "Building" + "/" + ns["building"] + "lod2Solid" + "[1]";
  print_info_aligned("Building stored in gml:Solid", doc.select_nodes(s.c_str()).size(), true);
  
  s = "//" + ns["building"] + "Building";
  pugi::xpath_node_set nb = doc.select_nodes(s.c_str());
  int total = 0;
  for (auto& b : nb) {
    std::string s1 = ".//" + ns["building"] + "lod2MultiSurface";
    pugi::xpath_node_set tmp = b.node().select_nodes(s1.c_str());
    if (tmp.empty() == false) {
      for (auto& nbp : tmp) {
        total++;
        break;
      }
    }
  }
  print_info_aligned("Building stored in gml:MultiSurface", total, true);
  // int total = 0;
  // for (auto& b : nb) {
  //   std::string s1 = ".//" + ns["building"] + "lod2MultiSurface";
  //   pugi::xpath_node_set tmp = b.node().select_nodes(s1.c_str());
  //   if (tmp.empty() == false) {
  //     for (auto& nbp : tmp) {
  //       total++;
  //       break;
  //     }
  //   }
  // }
  // print_info_aligned("Building with LOD2 semantics", total, true);

  s = "//" + ns["building"] + "Building" + "/" + ns["building"] + "boundedBy" + "[1]";
  print_info_aligned("Building with semantics", doc.select_nodes(s.c_str()).size(), true);


  // s = "//" + ns["building"] + "Building" + "/" + ns["building"] + "lod2MultiSurface" + "[1]";
  // print_info_aligned("Building stored in gml:MultiSurface", doc.select_nodes(s.c_str()).size(), true);

  // s = "//" + ns["building"] + "Building" + "/" + ns["building"] + "boundedBy" + "/" + ns["building"] + "RoofSurface" + "/" + ns["building"] + "lod2MultiSurface" + "[1]";
  // print_info_aligned("Building stored in gml:MultiSurface", doc.select_nodes(s.c_str()).size(), true);
  
  s = "//" + ns["building"] + "Building" + "/" + ns["building"] + "consistsOfBuildingPart" + "/" + ns["building"] + "BuildingPart" + "/" + ns["building"] + "lod2Solid" + "[1]";
  print_info_aligned("BuildingPart stored in gml:Solid", doc.select_nodes(s.c_str()).size(),true);

  std::cout << "LOD3" << std::endl;
  s = "//" + ns["building"] + "Building" + "/" + ns["building"] + "lod3Solid" + "[1]";
  print_info_aligned("Building stored with gml:Solid", doc.select_nodes(s.c_str()).size(), true);
  s = "//" + ns["building"] + "Building" + "/" + ns["building"] + "consistsOfBuildingPart" + "/" + ns["building"] + "BuildingPart" + "/" + ns["building"] + "lod3Solid" + "[1]";
  print_info_aligned("BuildingPart stored with gml:Solid", doc.select_nodes(s.c_str()).size(), true);
  


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

