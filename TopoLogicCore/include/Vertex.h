#pragma once

#include "Topology.h"
#include "Utilities.h"

#include <Standard_Handle.hxx>
#include <Geom_Geometry.hxx>
#include <TopoDS_Vertex.hxx>

class Geom_Point;
class gp_Pnt;

namespace TopoLogicCore
{
	class Edge;

	/// <summary>
	/// The representation of a topological vertex. This class wraps OCCT's TopoDS_Vertex.
	/// </summary>
	class Vertex : public Topology
	{
	public:
		/// <summary>
		/// Creates a vertex by an OCCT vertex.
		/// </summary>
		/// <param name="rkOcctVertex">An OCCT vertex</param>
		/// <param name="rkOcctLabel">Pass this if this is based on a vertex already in the LabelManager. 
		/// Otherwise a default value is used and the label will be added to the LabelManager.</param>
		Vertex(const TopoDS_Vertex& rkOcctVertex, const TDF_Label& rkOcctLabel = TDF_Label());

		virtual ~Vertex();

		/// <summary>
		/// Creates a vertex by a point.
		/// </summary>
		/// <param name="pOcctPoint">An OCCT point</param>
		/// <returns>The created TopoLogic vertex.</returns>
		static TOPOLOGIC_API std::shared_ptr<Vertex> ByPoint(Handle(Geom_Point) pOcctPoint);

		/// <summary>
		/// Returns the list of edges of which this vertex is a constituent member.
		/// </summary>
		/// <param name="rEdges">The edges containing this vertex as a constituent member</param>
		void TOPOLOGIC_API Edges(std::list<std::shared_ptr<Edge>>& rEdges);

		/// <summary>
		/// Return the corresponding point of this vertex. The output list only contains one vertex.
		/// </summary>
		/// <exception cref="std::exception">Null OCCT vertex</exception>
		/// <param name="rOcctGeometries">The output parameter, containing only one point corresponding to this vertex.</param>
		virtual void Geometry(std::list<Handle(Geom_Geometry)>& rOcctGeometries) const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual TopoDS_Shape& GetOcctShape();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual const TopoDS_Shape& GetOcctShape() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual TopoDS_Vertex& GetOcctVertex();

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		virtual const TopoDS_Vertex& GetOcctVertex() const;

		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		Handle(Geom_Point) Point() const;

		virtual TopologyType GetType() const { return TOPOLOGY_VERTEX; }

	protected:
		/// <summary>
		/// The underlying OCCT vertex.
		/// </summary>
		TopoDS_Vertex m_occtVertex;
	};
}