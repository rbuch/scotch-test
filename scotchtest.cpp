#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <vector>

#include <scotch.h>

void work(SCOTCH_Num baseval, SCOTCH_Num vertnbr, SCOTCH_Num edgenbr,
          const std::vector<int>& fVerttab, const std::vector<int>& fEdgetab,
          const std::vector<int>& fEdlotab)
{
  const int numProcs = 32;
  /* adjacency list */
  SCOTCH_Num *verttab = (SCOTCH_Num *)malloc(sizeof(SCOTCH_Num) * (vertnbr+1));
  /* loads of vertices */
  SCOTCH_Num *velotab = (SCOTCH_Num *)malloc(sizeof(SCOTCH_Num) * vertnbr);
  /* id of the neighbors */
  SCOTCH_Num *edgetab = (SCOTCH_Num *)malloc(sizeof(SCOTCH_Num) * edgenbr);
  /* number of bytes exchanged */
  SCOTCH_Num *edlotab = (SCOTCH_Num *)malloc(sizeof(SCOTCH_Num) * edgenbr);

  // int edgeNum = 0;
  // double ratio = 256.0/maxLoad;
  // double byteRatio = 1024.0/maxBytes;

  // for(i = baseval; i < vertnbr; i++) {
  //   verttab[i] = edgeNum;
  //   velotab[i] = (SCOTCH_Num) ceil(ogr->vertices[i].getVertexLoad() * ratio);
  //   for(j = 0; j < ogr->vertices[i].sendToList.size(); j++) {
  //     edgetab[edgeNum] = ogr->vertices[i].sendToList[j].getNeighborId();
  //     edlotab[edgeNum] = (int) ceil(ogr->vertices[i].sendToList[j].getNumBytes() * byteRatio);
  //     edgeNum++;
  //   }
  //   for(j = 0; j < ogr->vertices[i].recvFromList.size(); j++) {
  //     edgetab[edgeNum] = ogr->vertices[i].recvFromList[j].getNeighborId();
  //     edlotab[edgeNum] = (int) ceil(ogr->vertices[i].recvFromList[j].getNumBytes() * byteRatio);
  //     edgeNum++;
  //   }
  // }
  // verttab[i] = edgeNum;
  // CkAssert(edgeNum == edgenbr);

  for (size_t i = 0; i < fVerttab.size(); i++)
  {
    verttab[i] = fVerttab[i];
  }

  for (size_t i = 0; i < fVerttab.size() - 1; i++)
  {
    velotab[i] = 200;
  }

  for (size_t i = 0; i < fEdgetab.size(); i++)
  {
    edgetab[i] = fEdgetab[i];
  }

  for (size_t i = 0; i < fEdlotab.size(); i++)
  {
    edlotab[i] = fEdlotab[i];
  }

  SCOTCH_Graph graph;		// Graph to partition
  SCOTCH_Strat strat;		// Strategy to achieve partitioning

  /* Initialize data structures */
  SCOTCH_graphInit (&graph);
  SCOTCH_stratInit (&strat);

  SCOTCH_graphBuild (&graph, baseval, vertnbr, verttab, NULL, velotab, NULL, edgenbr, edgetab, edlotab);
  SCOTCH_graphCheck (&graph);

  SCOTCH_stratGraphMapBuild (&strat, SCOTCH_STRATBALANCE, numProcs, 0.01);
  SCOTCH_Num *pemap = (SCOTCH_Num *)malloc(sizeof(SCOTCH_Num) * vertnbr);

  SCOTCH_graphPart(&graph, numProcs, &strat, pemap);

  for (int i = 0; i < vertnbr; i++)
  {
    std::cout << pemap[i] << std::endl;
  }

  SCOTCH_graphExit (&graph);
  SCOTCH_stratExit (&strat);

  free(verttab);
  free(velotab);
  free(edgetab);
  free(edlotab);
  free(pemap);
}

template <typename T>
void readVec(std::ifstream& file, std::vector<T>& vec, int count)
{
  std::string dummy;
  file >> dummy; // eat header
  for (int i = 0; i < count; i++)
  {
    int dummyVal;
    T val;
    file >> dummyVal >> dummy >> val;
    vec.emplace_back(val);
  }
}

int main(int argc, char* argv[])
{
  // Read from file provided on command line
  if (argc < 2)
  {
    std::cerr << "Please provide filename." << std::endl;
    return -1;
  }
  std::ifstream file(argv[1], std::ios::in);
  if (!file.is_open())
  {
    std::cerr << "Failed to open " << argv[1] << std::endl;
    return -2;
  }
  std::string dummy;
  int edgeNum, baseval, vertnbr, edgenbr;
  double ratio, byteRatio;
  std::vector<int> verttab, edgetab, edlotab;
  file >> dummy >> edgeNum;
  file >> dummy >> ratio;
  file >> dummy >> byteRatio;
  file >> dummy >> baseval;
  file >> dummy >> vertnbr;
  file >> dummy >> edgenbr;

  readVec(file, verttab, vertnbr + 1);
  readVec(file, edgetab, edgenbr);
  readVec(file, edlotab, edgenbr);

  file.close();

  work(baseval, vertnbr, edgenbr, verttab, edgetab, edlotab);

  return 0;
}
