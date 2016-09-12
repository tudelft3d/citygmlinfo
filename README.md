# citygmlinfo

A very simple and lean program that simply takes as input a CityGML file and outputs some information about it.

I use it to get an insight into big datasets that are otherwise impossible (or almost) to obtain with any viewer or software.
It can easily parse and report on massive CityGML files (such as [Berlin](http://www.businesslocationcenter.de/en/downloadportal)), and it's rather fast.

The following CityGML classes are supported:

  - Geometric Primitives (`-P`)
  - Building (`-B`)
  - Relief (`-R`)
  - LandUse (`-L`)

I'll add other classes at some point.

```
$ ./citygmlinfo /data/Charlottenburg-Wilmersdorf/citygml.gml

Reading file: /data/Charlottenburg-Wilmersdorf/citygml.gml... done.

++++++++++++++++++++ GENERAL +++++++++++++++++++++
CityGML version: v2.0
CityGML classes present:
    Building
```

```
$ ./citygmlinfo /data/Charlottenburg-Wilmersdorf/citygml.gml -G -B

Reading file: citygml.gml... done.

++++++++++++++++++++ GENERAL +++++++++++++++++++++
CityGML version: v2.0
CityGML classes present:
    Building
    
+++++++++++++++++++ PRIMITIVES +++++++++++++++++++
gml:Solid                                    2,353
gml:MultiSolid                                   0
gml:CompositeSolid                               0
gml:MultiSurface                           167,317
gml:CompositeSurface                         2,353
gml:Polygon                                428,581

++++++++++++++++++++ BUILDINGS +++++++++++++++++++
Building                                    22,771
    without BuildingPart                    22,768
    having BuildingPart                          3
    with gml:id                             22,771
BuildingPart                                    34
    with gml:id                                 34
LOD0
    Building with FootPrint                      0
    Building with RoofEdge                       0
LOD1
    Building stored in gml:Solid                 0
    Building stored in gml:MultiSurface          0
    Building with semantics for surfaces         0
LOD2
    Building stored in gml:Solid             2,322
    Building stored in gml:MultiSurface     22,771
    Building with semantics for surfaces    22,771
LOD3
    Building stored in gml:Solid                 0
    Building stored in gml:MultiSurface          0
    Building with semantics for surfaces         0
LOD4
    Building stored in gml:Solid                 0
    Building stored in gml:MultiSurface          0
    Building with semantics for surfaces         0
Terrain Intersection Curve
    Building with lod1 TIC                       0
    Building with lod2 TIC                       0
    Building with lod3 TIC                       0
    Building with lod4 TIC                       0
```