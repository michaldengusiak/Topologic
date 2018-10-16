#include "CellUtility.h"
#include "Wire.h"
#include "Vertex.h"/*
#include "Face.h"
#include "Edge.h"*/

namespace Topologic {
	namespace Support {

		Cell^ CellUtility::ByVerticesFaceIndices(System::Collections::Generic::IEnumerable<Vertex^>^ vertices, System::Collections::Generic::IEnumerable<System::Collections::Generic::IEnumerable<int>^>^ faceIndices)
		{
			std::vector<TopologicCore::Vertex::Ptr> coreVertices;
			for each(Vertex^ pVertex in vertices)
			{
				coreVertices.push_back(TopologicCore::Topology::Downcast<TopologicCore::Vertex>(pVertex->GetCoreTopologicalQuery()));
			}

			std::list<std::list<int>> coreFaceIndices;
			for each(System::Collections::Generic::IEnumerable<int>^ pFaceIndex in faceIndices)
			{
				std::list<int> coreFaceIndex;
				for each(int vertexIndex in pFaceIndex)
				{
					coreFaceIndex.push_back(vertexIndex);
				}
				coreFaceIndices.push_back(coreFaceIndex);
			}

			Cell^ pCell = gcnew Cell(TopologicCore::Cell::ByVerticesFaceIndices(coreVertices, coreFaceIndices));
			return pCell;
		}

		Cell^ CellUtility::ByLoft(System::Collections::Generic::IEnumerable<Wire^>^ wires)
		{

			std::list<TopologicCore::Wire::Ptr> coreWires;
			for each(Wire^ pWire in wires)
			{
				coreWires.push_back(TopologicCore::Topology::Downcast<TopologicCore::Wire>(pWire->GetCoreTopologicalQuery()));
			}

			try {
				TopologicCore::Cell::Ptr pCoreCell = TopologicCore::Cell::ByLoft(coreWires);
				return gcnew Cell(pCoreCell);
			}
			catch (std::exception&)
			{
				throw gcnew Exception("Loft error");
			}
		}

		double CellUtility::Volume(Cell^ cell)
		{
			TopologicCore::Cell::Ptr pCoreCell = TopologicCore::Topology::Downcast<TopologicCore::Cell>(cell->GetCoreTopologicalQuery());
			return pCoreCell->Volume();
		}

		bool CellUtility::Contains(Cell^ cell, Vertex ^ vertex)
		{
			TopologicCore::Cell::Ptr pCoreCell = TopologicCore::Topology::Downcast<TopologicCore::Cell>(cell->GetCoreTopologicalQuery());
			TopologicCore::Vertex::Ptr pCoreVertex = TopologicCore::Topology::Downcast<TopologicCore::Vertex>(vertex->GetCoreTopologicalQuery());
			return pCoreCell->DoesContain(pCoreVertex);
		}

	}
}