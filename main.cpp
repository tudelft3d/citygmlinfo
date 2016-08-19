




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
void        report_building_each_lod(pugi::xml_document& doc, std::map<std::string, std::string>& ns, int lod, int& total_solid, int& total_ms, int& total_sem);
void        report_primitives(pugi::xml_document& doc, std::map<std::string, std::string>& ns);
void        print_info_aligned(std::string o, size_t number, bool tab = false);
void        get_namespaces(pugi::xml_node& root, std::map<std::string, std::string>& ns, std::string& vcitygml);



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

  TCLAP::CmdLine cmd("Allowed options", ' ', "0.2");
  // MyOutput my;
  // cmd.setOutput(&my);
  try {
    TCLAP::UnlabeledValueArg<std::string>  inputfile("inputfile", "The CityGML file", true, "", "string");
    // TCLAP::SwitchArg                       buildings("B", "Buildings", "info about the Buildings", false);
    // TCLAP::SwitchArg                       geomprimitives("P", "geomprimitives", "unit tests output", false);
    TCLAP::SwitchArg                       verbose("", "verbose", "verbose output", false);

    // cmd.add(buildings);
    // cmd.add(geomprimitives);
    cmd.add(verbose);
    cmd.add(inputfile);
    cmd.parse( argc, argv );

    std::cout << "Reading file: " << inputfile.getValue() << "... " << std::flush;
    pugi::xml_document doc;
    if (!doc.load_file(inputfile.getValue().c_str())) {
      std::cerr << "File not found" << std::endl;
      return 0;
    }
    std::cout << "done." << std::endl << std::endl;

    //-- parse namespace
    pugi::xml_node ncm = doc.first_child();
    std::string vcitygml;
    get_namespaces(ncm, ns, vcitygml);

    if (vcitygml.empty() == true) {
      std::cerr << "File does not have the CityGML namespace. Abort." << std::endl;
      return 0;
    }
    std::cout << "++++++++++++++++++++ GENERAL +++++++++++++++++++++" << std::endl;
    std::cout << "CityGML version: " << vcitygml << std::endl << std::endl;


    report_primitives(doc, ns);
    report_building(doc, ns);
    
    return 1;
  }
  catch (TCLAP::ArgException &e) {
    std::cout << "ERROR: " << e.error() << " for arg " << e.argId() << std::endl;
    return(0);
  }
}


void get_namespaces(pugi::xml_node& root, std::map<std::string, std::string>& ns, std::string& vcitygml) {
  vcitygml = "";
  for (pugi::xml_attribute attr = root.first_attribute(); attr; attr = attr.next_attribute()) {
    std::string name = attr.name();
    if (name.find("xmlns") != std::string::npos) {
      // std::cout << attr.name() << "=" << attr.value() << std::endl;
      std::string value = attr.value();
      std::string sns;
      if (value.find("http://www.opengis.net/citygml/0") != std::string::npos) {
        sns = "citygml";
        vcitygml = "v0.4";
      }
      else if (value.find("http://www.opengis.net/citygml/1") != std::string::npos) {
        sns = "citygml";
        vcitygml = "v1.0";
      }
      else if (value.find("http://www.opengis.net/citygml/2") != std::string::npos) {
        sns = "citygml";
        vcitygml = "v2.0";
      }
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


void report_building_each_lod(pugi::xml_document& doc, std::map<std::string, std::string>& ns, int lod, int& total_solid, int& total_ms, int& total_sem) {
  total_solid = 0;
  total_ms = 0;
  total_sem = 0;
  std::string slod = "lod" + std::to_string(lod);
  std::string s = "//" + ns["building"] + "Building";
  pugi::xpath_node_set nb = doc.select_nodes(s.c_str());
  for (auto& b : nb) {
    std::string s1 = ".//" + ns["building"] + slod + "Solid";
    pugi::xpath_node_set tmp = b.node().select_nodes(s1.c_str());
    if (tmp.empty() == false) {
      for (auto& nbp : tmp) {
        total_solid++;
        break;
      }
    }
    s1 = ".//" + ns["building"] + slod + "MultiSurface";
    tmp = b.node().select_nodes(s1.c_str());
    if (tmp.empty() == false) {
      for (auto& nbp : tmp) {
        total_ms++;
        break;
      }
    }
    s1 = ".//" + ns["building"] + "boundedBy" + "//" + ns["building"] + slod + "MultiSurface";
    tmp = b.node().select_nodes(s1.c_str());
    if (tmp.empty() == false) {
      for (auto& nbp : tmp) {
        total_sem++;
        break;
      }
    }
  }
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
  
  std::cout << "LOD0" << std::endl;
  int total_footprint = 0;
  int total_roofedge = 0;
  s = "//" + ns["building"] + "Building";
  pugi::xpath_node_set nb = doc.select_nodes(s.c_str());
  for (auto& b : nb) {
    std::string s1 = ".//" + ns["building"] + "lod0FootPrint";
    pugi::xpath_node_set tmp = b.node().select_nodes(s1.c_str());
    if (tmp.empty() == false) {
      for (auto& nbp : tmp) {
        total_footprint++;
        break;
      }
    }
    s1 = ".//" + ns["building"] + "lod0RoofEdge";
    tmp = b.node().select_nodes(s1.c_str());
    if (tmp.empty() == false) {
      for (auto& nbp : tmp) {
        total_roofedge++;
        break;
      }
    }
  }
  print_info_aligned("Building with FootPrint", total_footprint, true);
  print_info_aligned("Building with RoofEdge", total_roofedge, true);
  
  for (int lod = 1; lod <= 4; lod++) {
    std::cout << "LOD" << lod << std::endl;
    int totals = 0;
    int totalms = 0;
    int totalsem = 0;
    report_building_each_lod(doc, ns, lod, totals, totalms, totalsem);
    print_info_aligned("Building stored in gml:Solid", totals, true);
    print_info_aligned("Building stored in gml:MultiSurface", totalms, true);
    print_info_aligned("Building with semantics for surfaces", totalsem, true);
  }


  std::cout << std::endl;
}


//-- ignore XML namespace
std::string localise(std::string s) {
  return "*[local-name(.) = '" + s + "']";
}

