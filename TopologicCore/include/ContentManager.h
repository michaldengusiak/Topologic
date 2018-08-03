#pragma once

#include "Utilities.h"

#include <TopoDS_Shape.hxx>

#include <list>
#include <map>
#include <memory>

namespace TopologicCore
{
	class Topology;

	class ContentManager
	{
	public:
		typedef std::shared_ptr<ContentManager> Ptr;

	public:
		static ContentManager& GetInstance()
		{
			static ContentManager instance;
			return instance;
		}

		void Add(const TopoDS_Shape& rkOcctShape, const std::shared_ptr<Topology>& kpContentTopology);

		//void Remove(const TopoDS_Shape& rkOcctShape, const std::shared_ptr<Topology>& kpContentTopology);
		void Remove(const TopoDS_Shape& rkOcctShape, const TopoDS_Shape& rkOcctContentTopology);

		bool Find(const TopoDS_Shape& rkOcctShape, std::list<std::shared_ptr<Topology>>& rContents);

		void ClearOne(const TopoDS_Shape& rkOcctShape);

		void ClearAll();

	protected:
		std::map<TopoDS_Shape, std::list<std::shared_ptr<Topology>>, OcctShapeComparator> m_occtShapeToContentsMap;
	};
}