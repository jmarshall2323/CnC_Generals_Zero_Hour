/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : WW3D                                                         *
 *                                                                                             *
 *                     $Archive:: /Commando/Code/ww3d2/decalmsh.cpp                           $*
 *                                                                                             *
 *              Original Author:: Greg Hjelstrom                                               *
 *                                                                                             *
 *                      $Author:: Greg_h                                                      $*
 *                                                                                             *
 *                     $Modtime:: 7/26/01 9:03a                                               $*
 *                                                                                             *
 *                    $Revision:: 20                                                          $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   DecalMeshClass::DecalMeshClass -- Constructor                                             *
 *   DecalMeshClass::~DecalMeshClass -- Destructor                                             *
 *   RigidDecalMeshClass::RigidDecalMeshClass -- Constructor                                   *
 *   RigidDecalMeshClass::~RigidDecalMeshClass -- Destructor                                   *
 *   RigidDecalMeshClass::Render -- Render the decals                                          *
 *   RigidDecalMeshClass::Process_Material_Run -- scans the mesh for material runs             *
 *   RigidDecalMeshClass::Create_Decal -- Generate a new decal                                 *
 *   RigidDecalMeshClass::Delete_Decal -- Delete a decal                                       *
 *   SkinDecalMeshClass::SkinDecalMeshClass -- Constructor                                     *
 *   SkinDecalMeshClass::~SkinDecalMeshClass -- Destructor                                     *
 *   SkinDecalMeshClass::Render -- Render the decals                                           *
 *   SkinDecalMeshClass::Create_Decal -- Generate a new decal                                  *
 *   SkinDecalMeshClass::Delete_Decal -- Delete a decal                                        *
 *   SkinDecalMeshClass::Process_Material_Run -- scans the mesh for material runs              *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "decalmsh.h"

#include <vector>

#include "decalsys.h"
#include "rinfo.h"
#include "mesh.h"
#include "meshmdl.h"
#include "plane.h"
#include "statistics.h"
#include "GameRenderer.h"
#include "texture.h"

#define DISABLE_CLIPPING	0

/**
** DecalPolyClass - This class is used to clip polygons as they are
** added to a RigidDecalMesh. 
**
** Data needed to add a poly to the decal mesh:
** connectivity - generated on the fly after the poly is clipped
** planeeq - constant for entire poly, copy from source after done
** verts - plug into DecalPolyClass, clip, pull back out
** vnorms - plug into DecalPolyClass, clip, copy back out 
** texcoords - compute after poly is clipped
** material - contstant for entire poly, get from generator
** shader - constant for entire poly, get from generator
** texture - constant for entire poly, get from generator
*/
class DecalPolyClass
{
public:
	void Reset(void);
	void Add_Vertex(const Vector3 & point,const Vector3 & normal);
	void Clip(const PlaneClass & plane,DecalPolyClass & dest) const;

	std::vector<Vector3> Verts;
	std::vector<Vector3> VertNorms;
};


void DecalPolyClass::Reset(void)
{
	Verts.clear();
	VertNorms.clear();
}

void DecalPolyClass::Add_Vertex(const Vector3 & point,const Vector3 & norm)
{
	Verts.push_back(point);
	VertNorms.push_back(norm);
}

void DecalPolyClass::Clip(const PlaneClass & plane,DecalPolyClass & dest) const
{
	dest.Reset();

	if (Verts.size() <= 2) return;

	// temporary variables used in clipping
	size_t i = 0;
	size_t iprev = Verts.size() - 1;
	bool cur_point_in_front;
	bool prev_point_in_front;
	
	float alpha;
	Vector3 int_point;
	Vector3 int_normal;

	// perform clipping
	prev_point_in_front = plane.In_Front(Verts[iprev]);
#if DISABLE_CLIPPING
	prev_point_in_front = true;
#endif

	for (size_t j = 0; j < Verts.size(); ++j)
	{
		
		cur_point_in_front = plane.In_Front(Verts[i]);
#if DISABLE_CLIPPING
		cur_point_in_front = true;		
#endif

		if (prev_point_in_front)
		{
			if (cur_point_in_front)
			{
				// Previous vertex was in front of plane and this vertex is in
				// front of the plane so we emit this vertex.
				dest.Add_Vertex(Verts[i],VertNorms[i]);
			}
			else
			{
				// Previous vert was in front, this vert is behind, compute
				// the intersection and emit the point.
				plane.Compute_Intersection(Verts[iprev],Verts[i],&alpha);
				Vector3::Lerp(Verts[iprev],Verts[i],alpha,&int_point);
				Vector3::Lerp(VertNorms[iprev],VertNorms[i],alpha,&int_normal);
				dest.Add_Vertex(int_point,int_normal);
			}
		}
		else
		{
			if (cur_point_in_front)
			{
				// segment is going from the back halfspace to the front halfspace
				// compute the intersection and emit it, then continue
				// the edge into the front halfspace and emit the end point.
				plane.Compute_Intersection(Verts[iprev],Verts[i],&alpha);
				Vector3::Lerp(Verts[iprev],Verts[i],alpha,&int_point);
				Vector3::Lerp(VertNorms[iprev],VertNorms[i],alpha,&int_normal);
				dest.Add_Vertex(int_point,int_normal);
				dest.Add_Vertex(Verts[i],VertNorms[i]);
			}
		}

		prev_point_in_front = cur_point_in_front;
		iprev = i;
		i = (i+1)%(Verts.size());
	}
}

static DecalPolyClass _DecalPoly0;
static DecalPolyClass _DecalPoly1;


/*
** DecalMeshClass Implementation
*/

/***********************************************************************************************
 * DecalMeshClass::DecalMeshClass -- Constructor                                               *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/26/00    gth : Created.                                                                 *
 *=============================================================================================*/
DecalMeshClass::DecalMeshClass(MeshClass * parent,DecalSystemClass * system) :
	Parent(parent),
	DecalSystem(system)
{
	WWASSERT(Parent != NULL);
	WWASSERT(DecalSystem != NULL);
}


/***********************************************************************************************
 * DecalMeshClass::~DecalMeshClass -- Destructor                                               *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/26/00    gth : Created.                                                                 *
 *=============================================================================================*/
DecalMeshClass::~DecalMeshClass(void)
{
}


/*
** RigidDecalMeshClass Implementation
*/


/***********************************************************************************************
 * RigidDecalMeshClass::RigidDecalMeshClass -- Constructor                                     *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/31/00    NH : Created.                                                                  *
 *=============================================================================================*/
RigidDecalMeshClass::RigidDecalMeshClass(MeshClass * parent, DecalSystemClass * system) :
	DecalMeshClass(parent, system)
{
}


/***********************************************************************************************
 * RigidDecalMeshClass::~RigidDecalMeshClass -- Destructor                                     *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/31/00    NH : Created.                                                                  *
 *=============================================================================================*/
RigidDecalMeshClass::~RigidDecalMeshClass(void)
{
	// Notify the system that this decal mesh is being destroyed.
	for (const auto& decal : Decals)
	{
		DecalSystem->Decal_Mesh_Destroyed(decal.DecalID, this);
	}
	// Release all of our references.  The memory in the arrays will automatically be 
	// released by the std::vector...
	for (size_t i = 0; i < Polys.size(); i++)
	{
		REF_PTR_RELEASE(Textures[i]);
	}

	for (size_t i = 0; i < Verts.size(); i++)
	{
		REF_PTR_RELEASE(VertexMaterials[i]);
	}
}


/***********************************************************************************************
 * RigidDecalMeshClass::Render -- Render the decals                                            *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/26/00    gth : Created.                                                                 *
 *=============================================================================================*/
void RigidDecalMeshClass::Render(void)
{
	if ((Decals.size() == 0) || (WW3D::Are_Decals_Enabled() == false)) return;
	
	/*
	** Install the mesh'es transform.  NOTE, this could go wrong if someone changes the
	** transform between the time that the mesh is rendered and the time that the decal
	** mesh is rendered...  It shouldn't happen though.
	*/
	DX8Wrapper::Set_Transform(D3DTS_WORLD,Parent->Get_Transform());

	/*
	** Copy the vertices into the dynamic vb
	*/
	DynamicVBAccessClass dynamic_vb(BUFFER_TYPE_DYNAMIC_DX8, dynamic_fvf_type, Verts.size());
	{	
		DynamicVBAccessClass::WriteLockClass lock(&dynamic_vb);
		VertexFormatXYZNDUV2 * vertex = lock.Get_Formatted_Vertex_Array();

		for (size_t i = 0; i < Verts.size(); i++)
		{
			vertex->x = Verts[i].X;
			vertex->y = Verts[i].Y;
			vertex->z = Verts[i].Z;

			vertex->nx = VertNorms[i].X;
			vertex->ny = VertNorms[i].Y;
			vertex->nz = VertNorms[i].Z;

			vertex->diffuse = 0xFFFFFFFF;

			vertex->u1 = TexCoords[i].X;
			vertex->v1 = TexCoords[i].Y;

			vertex->u2 = 0.0f;
			vertex->v2 = 0.0f;

			vertex++;
		}
	}
	
	/*
	** Copy the indices into the dynamic ib
	*/
	DynamicIBAccessClass dynamic_ib(BUFFER_TYPE_DYNAMIC_DX8, Polys.size() * 3);
	{
		DynamicIBAccessClass::WriteLockClass lock(&dynamic_ib);
		unsigned short * indices = lock.Get_Index_Array();
		for (size_t i = 0; i < Polys.size(); i++)
		{
			indices[i*3 + 0] = (unsigned short)Polys[i].I;
			indices[i*3 + 1] = (unsigned short)Polys[i].J;
			indices[i*3 + 2] = (unsigned short)Polys[i].K;
		}
	}

	/*
	** Render in runs of constant material settings
	*/
	size_t cur_poly_index = 0;
	size_t next_poly_index = 0;

	while (next_poly_index < Polys.size())
	{
		next_poly_index = Process_Material_Run(cur_poly_index);

		DX8Wrapper::Set_Index_Buffer(dynamic_ib,0);
		DX8Wrapper::Set_Vertex_Buffer(dynamic_vb);
		DX8Wrapper::Draw_Triangles(	3*cur_poly_index,
												(next_poly_index - cur_poly_index), // poly count
												Polys[cur_poly_index].I, 
												1 + Polys[next_poly_index-1].K - Polys[cur_poly_index].I);
		cur_poly_index = next_poly_index;
	}
		
}


/***********************************************************************************************
 * RigidDecalMeshClass::Process_Material_Run -- scans the mesh for material runs               *
 *                                                                                             *
 *    This function will install the materials for poly[start_index] and scan forward for      *
 *    the next material change.  It will return the start index for the next material change   *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   2/22/2001  gth : Created.                                                                 *
 *=============================================================================================*/
int RigidDecalMeshClass::Process_Material_Run(int start_index)
{
	DX8Wrapper::Set_Texture(0,Textures[start_index]);
	DX8Wrapper::Set_Material(VertexMaterials[Polys[start_index].I]);
	DX8Wrapper::Set_Shader(Shaders[start_index]);

	size_t next_index = start_index;
	while ((next_index < Polys.size()) &&
	       (Textures[next_index] == Textures[start_index]) &&
	       (Shaders[next_index] == Shaders[start_index]) &&
	       (VertexMaterials[next_index] == VertexMaterials[start_index]))
	{
		next_index++;
	}
	return next_index;
}

/***********************************************************************************************
 * RigidDecalMeshClass::Create_Decal -- Generate a new decal                                   *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * All Decals on a mesh must be generated from the same DecalSystemClass!                      *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/26/00    gth : Created.                                                                 *
 *=============================================================================================*/
bool RigidDecalMeshClass::Create_Decal
(
	DecalGeneratorClass* generator,
	const OBBoxClass& localbox,
	std::vector<uint32>& apt,
	const std::vector<Vector3>* world_vertex_locs
)
{
	// Since we can't rely on the hardware polygon offset function, I'm physically offsetting
	// the decal polygons along the normal of the decal generator.  If we could instead rely
	// on hardware "polygon offset" we could remove this code and we could make decals non-sorting
#if 0
	const float ZBIAS_DISTANCE = 0.01f;
	Vector3 zbias_offset;
	generator->Get_Transform().Get_Z_Vector(&zbias_offset);
	Matrix3D invtm;
	Parent->Get_Transform().Get_Orthogonal_Inverse(invtm);
	Matrix3D::Rotate_Vector(invtm,zbias_offset,&zbias_offset);
	zbias_offset *= ZBIAS_DISTANCE;
#endif

	// NOTE: world_vertex_locs/norms should not be set for this class
	WWASSERT(world_vertex_locs == 0);

	WWASSERT(generator->Peek_Decal_System() == DecalSystem);
	
	/*
	** If any polys were collected, add a new MeshDecalStruct
	*/
	if (apt.size() == 0)
		return false;

	DecalStruct newdecal;
	newdecal.DecalID = generator->Get_Decal_ID();
	newdecal.FaceStartIndex = Polys.size();   // start faces at the end of the current array
	newdecal.FaceCount = 0;								// init facecount to zero
	newdecal.VertexStartIndex = Verts.size(); // start vertices at the end of the current array
	newdecal.VertexCount = 0;							// init vertcount to zero
	
	/*
	** Grab pointers to the parent mesh's components
	*/
	MeshModelClass * model = Parent->Peek_Model();
	const Vector3i * src_polys		= model->Get_Polygon_Array();
	const Vector3 * src_verts		= model->Get_Vertex_Array();
	const Vector3 * src_vnorms		= model->Get_Vertex_Normal_Array();

	/*
	** Grab a pointer to the material settings
	*/
	MaterialPassClass * material = generator->Get_Material();
	
	/*
	** Set up the generator for our coordinate system
	*/
	generator->Set_Mesh_Transform(Parent->Get_Transform());
	
	/*
	** Compute the clipping planes
	*/
	PlaneClass planes[4];
	Vector3 extent;

	Matrix3::Rotate_Vector(localbox.Basis,Vector3(localbox.Extent.X,0,0),&extent);
	Vector3 direction(localbox.Basis.Get_X_Vector());
	
	planes[0].Set(-direction,localbox.Center + extent);
	planes[1].Set(direction,localbox.Center - extent);
	
	Matrix3::Rotate_Vector(localbox.Basis,Vector3(0,localbox.Extent.Y,0),&extent);
	direction.Set(localbox.Basis.Get_Y_Vector());
	
	planes[2].Set(-direction,localbox.Center + extent);
	planes[3].Set(direction,localbox.Center - extent);

	/*
	** Generate the faces and per-face info
	*/
	bool added_polys = false;
	Vector3 pdir = localbox.Basis.Get_Z_Vector();

	for (const auto& active_poly : apt) {

		/*
		** check if the polygon is backfacing
		*/
		PlaneClass plane;
		model->Compute_Plane(active_poly, &plane);

		float dot = Vector3::Dot_Product(plane.N,pdir);
		if (dot > generator->Get_Backface_Threshhold()) {
			/*
			** Copy src_polys[active_poly] into our clip polygon
			*/
			_DecalPoly0.Reset();
			const Vector3i & poly = src_polys[active_poly];
			for (size_t j = 0; j < 3; j++)
			{
				_DecalPoly0.Add_Vertex(src_verts[poly[j]] /*+ zbias_offset*/,src_vnorms[poly[j]]);
			}

			/*
			** Clip against the edges of the bounding box
			*/
			_DecalPoly0.Clip(planes[0],_DecalPoly1);
			_DecalPoly1.Clip(planes[1],_DecalPoly0);
			_DecalPoly0.Clip(planes[2],_DecalPoly1);
			_DecalPoly1.Clip(planes[3],_DecalPoly0);

			/*
			** Check if the clipped polygon is empty or degenerate
			*/
			if (_DecalPoly0.Verts.size() >= 3)
			{

				/*
				** Extract triangles from the clipped polygon
				*/
				size_t first_vert = Verts.size();

				for (size_t j = 1; j < _DecalPoly0.Verts.size() - 1; j++)
				{
					/*
					** Check if this triangle is degenerate (Sutherland-Hodgeman can sometimes create degenerate tris)
					*/
					// TODO

					/*
					** Add the triangle, its plane equation, and the per-tri materials
					*/
					added_polys = true;
					Polys.push_back(Vector3i(first_vert, first_vert + j, first_vert + j + 1));
					Shaders.push_back(material->Peek_Shader());
					Textures.push_back(material->Get_Texture()); // Get_Texture gives us a reference...
				}

				/*
				** Extract verts from the clipped polygon
				*/
				for (size_t j = 0; j < _DecalPoly0.Verts.size(); j++)
				{
					Verts.push_back(_DecalPoly0.Verts[j]);
					_DecalPoly0.VertNorms[j].Normalize();
					VertNorms.push_back(_DecalPoly0.VertNorms[j]);
					VertexMaterials.push_back(material->Get_Material()); // Get_Material gives us a ref.

					/*
					** Compute the uv coordinates for this vertex
					*/
					Vector3 stq;
					generator->Compute_Texture_Coordinate(Verts.back(), &stq);
					TexCoords.push_back(Vector2(stq.X,stq.Y));
				}
			}
		}
	}

	if (added_polys) {
		newdecal.FaceCount = Polys.size() - newdecal.FaceStartIndex;
		newdecal.VertexCount = Verts.size() - newdecal.VertexStartIndex;
		Decals.push_back(newdecal);

		/*
		** tell the generator that we added a decal
		*/
		generator->Add_Mesh(Parent);
	} 

	material->Release_Ref();
	
#ifdef WWDEBUG	
	/*
	** Some paranoid debug code: ensure all tris have valid vertex indices
	*/
	const size_t vert_count = Verts.size();
	for (const auto& poly : Polys)
	{
		WWASSERT (poly.I < vert_count);
		WWASSERT (poly.I >= 0);
		WWASSERT (poly.J < vert_count);
		WWASSERT (poly.J >= 0);
		WWASSERT (poly.K < vert_count);
		WWASSERT (poly.K >= 0);
	}
#endif

	/*
	** Only return true if we actually added a decal
	*/
	return added_polys;
}


/***********************************************************************************************
 * RigidDecalMeshClass::Delete_Decal -- Delete a decal                                         *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/26/00    gth : Created.                                                                 *
 *=============================================================================================*/
bool RigidDecalMeshClass::Delete_Decal(uint32 id)
{
	/*
	** Find the MeshDecal which matches the given id
	*/
	size_t decal_index = -1;
	for (size_t i = 0; i < Decals.size(); i++)
	{
		if (Decals[i].DecalID == id) {
			decal_index = i;
			break;
		}
	}

	if (decal_index == -1) {
		return false;
	}

	DecalStruct * decal = &Decals[decal_index];

	/*
	** Remove all geometry used by this decal 
	*/
	Polys.erase(Polys.begin() + decal->FaceStartIndex, Polys.begin() + decal->FaceStartIndex + decal->FaceCount);
	Verts.erase(Verts.begin() + decal->VertexStartIndex, Verts.begin() + decal->VertexStartIndex + decal->VertexCount);
	VertNorms.erase(VertNorms.begin() + decal->VertexStartIndex, VertNorms.begin() + decal->VertexStartIndex + decal->VertexCount);
	TexCoords.erase(TexCoords.begin() + decal->VertexStartIndex, TexCoords.begin() + decal->VertexStartIndex + decal->VertexCount);

	/*
	** Re-index the remaining triangle vertex indices
	*/
	for (auto& poly : Polys)
	{
		if (poly.I > decal->VertexStartIndex) poly.I -= decal->VertexCount;
		if (poly.J > decal->VertexStartIndex) poly.J -= decal->VertexCount;
		if (poly.K > decal->VertexStartIndex) poly.K -= decal->VertexCount;
	}

	/*
	** Remove all materials used by this decal (remember to release refs!)
	*/
	for (int fi=decal->FaceStartIndex; fi<decal->FaceCount; fi++) {
		REF_PTR_RELEASE(Textures[fi]);
	}
	for (int vi=decal->VertexStartIndex; vi<decal->VertexCount; vi++) {
		REF_PTR_RELEASE(VertexMaterials[vi]);
	}
	Shaders.erase(Shaders.begin() + decal->FaceStartIndex, Shaders.begin() + decal->FaceStartIndex + decal->FaceCount);
	Textures.erase(Textures.begin() + decal->FaceStartIndex, Textures.begin() + decal->FaceStartIndex + decal->FaceCount);
	VertexMaterials.erase(VertexMaterials.begin() + decal->VertexStartIndex, VertexMaterials.begin() + decal->VertexStartIndex + decal->VertexCount);

	/*
	** Remove MeshDecal and refresh all other decal indices
	*/
	for (size_t di = decal_index + 1; di < Decals.size(); di++)
	{
		Decals[di].FaceStartIndex -= decal->FaceCount;
		Decals[di].VertexStartIndex -= decal->VertexCount;
	}
	Decals.erase(Decals.begin() + decal_index);

#ifdef WWDEBUG	
	/*
	** Some paranoid debug code: ensure all tris have valid vertex indices
	*/
	size_t poly_count = Polys.size();
	size_t vert_count = Verts.size();
	for (size_t poly_idx = 0; poly_idx < poly_count; poly_idx++)
	{
		WWASSERT (Polys[poly_idx].I < vert_count);
		WWASSERT (Polys[poly_idx].I >= 0);
		WWASSERT (Polys[poly_idx].J < vert_count);
		WWASSERT (Polys[poly_idx].J >= 0);
		WWASSERT (Polys[poly_idx].K < vert_count);
		WWASSERT (Polys[poly_idx].K >= 0);
	}
#endif

	return true;
}


/*
** Temporary Buffers
** These buffers are used by the skin code for temporary storage of the deformed vertices and 
** vertex normals.  
*/
static std::vector<Vector3>	_TempVertexBuffer;
static std::vector<Vector3>	_TempNormalBuffer;


/*
** SkinDecalMeshClass Implementation
*/


/***********************************************************************************************
 * SkinDecalMeshClass::SkinDecalMeshClass -- Constructor                                       *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/31/00    NH : Created.                                                                  *
 *=============================================================================================*/
SkinDecalMeshClass::SkinDecalMeshClass(MeshClass * parent, DecalSystemClass * system) :
	DecalMeshClass(parent, system)
{
}


/***********************************************************************************************
 * SkinDecalMeshClass::~SkinDecalMeshClass -- Destructor                                       *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/31/00    NH : Created.                                                                  *
 *=============================================================================================*/
SkinDecalMeshClass::~SkinDecalMeshClass(void)
{
	// Notify the system that this decal mesh is being destroyed.
	for (const auto& decal : Decals)
	{
		DecalSystem->Decal_Mesh_Destroyed(decal.DecalID, this);
	}

	// Release all of our references.  The memory in the arrays will automatically be 
	// released by the std::vector...
	for (size_t i = 0; i < Polys.size(); i++)
	{
		REF_PTR_RELEASE(Textures[i]);
	}

	for (size_t i = 0; i < ParentVertexIndices.size(); i++)
	{
		REF_PTR_RELEASE(VertexMaterials[i]);
	}
}


/***********************************************************************************************
 * SkinDecalMeshClass::Render -- Render the decals                                             *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/31/00    NH : Created.                                                                  *
 *=============================================================================================*/
void SkinDecalMeshClass::Render(void)
{
	if ((Decals.size() == 0) || (WW3D::Are_Decals_Enabled() == false)) return;

	/*
	** Don't allow decals on sorted meshes
	*/
	MeshModelClass * model = Parent->Peek_Model();
	if (model->Get_Flag(MeshModelClass::SORT)) {
		WWDEBUG_SAY(("ERROR: decals applied to a sorted mesh!\n"));
		return;
	}

	/*
	** Skin decals coordinates are in world space
	*/
	DX8Wrapper::Set_Transform(D3DTS_WORLD,Matrix3D::Identity);

	/*
	** Skin decals have to get the deformed vertices of their parent meshes.  For this
	** reason, decals on skins is not a very good idea...  
	*/
	_TempVertexBuffer.assign(model->Get_Vertex_Count(), Vector3());
	_TempNormalBuffer.assign(model->Get_Vertex_Count(), Vector3());

	Parent->Get_Deformed_Vertices(_TempVertexBuffer.data(), _TempNormalBuffer.data());

	/*
	** Copy the vertices into the dynamic vb
	*/
	DynamicVBAccessClass dynamic_vb(BUFFER_TYPE_DYNAMIC_DX8,dynamic_fvf_type, ParentVertexIndices.size());
	{	
		DynamicVBAccessClass::WriteLockClass lock(&dynamic_vb);
		VertexFormatXYZNDUV2 * vertex = lock.Get_Formatted_Vertex_Array();

		for (size_t i = 0; i < ParentVertexIndices.size(); i++)
		{
			const size_t src_i = ParentVertexIndices[i];
			vertex->x = _TempVertexBuffer[src_i].X;
			vertex->y = _TempVertexBuffer[src_i].Y;
			vertex->z = _TempVertexBuffer[src_i].Z;

			vertex->nx = _TempNormalBuffer[src_i].X;
			vertex->ny = _TempNormalBuffer[src_i].Y;
			vertex->nz = _TempNormalBuffer[src_i].Z;

			vertex->diffuse = 0xFFFFFFFF;

			vertex->u1 = TexCoords[i].X;
			vertex->v1 = TexCoords[i].Y;

			vertex->u2 = 0.0f;
			vertex->v2 = 0.0f;
			
			vertex++;
		}
	}

	/*
	** Copy the indices into the dynamic ib
	*/
	DynamicIBAccessClass dynamic_ib(BUFFER_TYPE_DYNAMIC_DX8, Polys.size() * 3);
	{
		DynamicIBAccessClass::WriteLockClass lock(&dynamic_ib);
		unsigned short * indices = lock.Get_Index_Array();
		for (size_t i = 0; i < Polys.size(); i++)
		{
			indices[i*3 + 0] = (unsigned short)Polys[i].I;
			indices[i*3 + 1] = (unsigned short)Polys[i].J;
			indices[i*3 + 2] = (unsigned short)Polys[i].K;
		}
	}

	/*
	** Render in runs of constant material settings
	*/
	size_t cur_poly_index = 0;
	size_t next_poly_index = 0;

	while (next_poly_index < Polys.size()) {
		next_poly_index = Process_Material_Run(cur_poly_index);

		DX8Wrapper::Set_Index_Buffer(dynamic_ib,0);
		DX8Wrapper::Set_Vertex_Buffer(dynamic_vb);
		DX8Wrapper::Draw_Triangles(3*cur_poly_index,
											(next_poly_index - cur_poly_index), // poly count
											Polys[cur_poly_index].I, 
											1 + Polys[next_poly_index-1].K - Polys[cur_poly_index].I);
		
		cur_poly_index = next_poly_index;
	}
}


/***********************************************************************************************
 * SkinDecalMeshClass::Process_Material_Run -- scans the mesh for material runs                *
 *                                                                                             *
 *    This function will install the materials for poly[start_index] and scan forward for      *
 *    the next material change.  It will return the start index for the next material change   *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   2/22/2001  gth : Created.                                                                 *
 *=============================================================================================*/
int SkinDecalMeshClass::Process_Material_Run(int start_index)
{
	DX8Wrapper::Set_Texture(0,Textures[start_index]);
	DX8Wrapper::Set_Material(VertexMaterials[Polys[start_index].I]);
	DX8Wrapper::Set_Shader(Shaders[start_index]);

	size_t next_index = start_index;
	while (	(next_index < Polys.size()) &&
	        (Textures[next_index] == Textures[start_index]) &&
	        (Shaders[next_index] == Shaders[start_index]) &&
	        (VertexMaterials[next_index] == VertexMaterials[start_index])
	      )
	{
		next_index++;
	}
	return next_index;
}


/***********************************************************************************************
 * SkinDecalMeshClass::Create_Decal -- Generate a new decal                                    *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 * All Decals on a mesh must be generated from the same DecalSystemClass!                      *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/31/00    NH : Created.                                                                  *
 *=============================================================================================*/
bool SkinDecalMeshClass::Create_Decal(DecalGeneratorClass* generator,
	const OBBoxClass& localbox, std::vector<uint32>& apt,
	const std::vector<Vector3>* world_vertex_locs)
{
	WWASSERT(generator->Peek_Decal_System() == DecalSystem);

	// The dynamically updated vertex locations are needed - we have no static geometry
	WWASSERT(world_vertex_locs);
	
	/*
	** If any polys were collected, add a new MeshDecalStruct
	*/
	if (apt.size() == 0)
		return false;

	DecalStruct newdecal;
	newdecal.DecalID = generator->Get_Decal_ID();
	newdecal.FaceStartIndex = Polys.size();                 // start faces at the end of the current array
	newdecal.FaceCount = 0;												// init facecount to zero
	newdecal.VertexStartIndex = ParentVertexIndices.size(); // start vertices at the end of the current array
	newdecal.VertexCount = 0;											// init vertcount to zero
	
	/*
	** Grab pointers to the parent mesh's components
	*/
	MeshModelClass * model = Parent->Peek_Model();
	const Vector3i * src_polys = model->Get_Polygon_Array();

	/*
	** Grab a pointer to the material settings
	*/
	MaterialPassClass * material = generator->Get_Material();
	
	/*
	** Set up the generator for the world coordinate system (the deformed vertices are in worldspace)
	*/
	generator->Set_Mesh_Transform(Matrix3D::Identity);
	
	/*
	** Generate the faces and per-face info (remember to add-ref's)
	** TODO: rewrite this to take advantage of vertex sharing...
	*/
	const size_t face_size_hint = Polys.size() + apt.size();

	Polys.reserve(face_size_hint);
	Shaders.reserve(face_size_hint);
	Textures.reserve(face_size_hint);

	const size_t first_vert = ParentVertexIndices.size();

	for (size_t i = 0; i < apt.size(); i++)
	{
		const size_t offset = first_vert + i * 3;
		Polys.push_back(Vector3i(offset, offset + 1, offset + 2));
		
		Shaders.push_back(material->Peek_Shader());
		Textures.push_back(material->Get_Texture()); // Get_Texture gives us a reference...
	}

	/*
	** Copy the vertices and per-vertex info
	** TODO: rewrite this to take advantage of vertex sharing...
	*/
	const size_t vertex_size_hint = ParentVertexIndices.size() + 3 * apt.size();

	ParentVertexIndices.reserve(vertex_size_hint);
	VertexMaterials.reserve(vertex_size_hint);
	TexCoords.reserve(vertex_size_hint);

	for (size_t i = 0; i < apt.size(); i++) {
		const size_t face_index = apt[i];
		for (int vi = 0; vi < 3; vi++) {

			/*
			** Copy data for this vertex
			*/
			ParentVertexIndices.push_back(src_polys[face_index][vi]);
			VertexMaterials.push_back(material->Get_Material()); // Get_Material gives us a ref.

			/*
			** Compute the uv coordinates for this vertex
			*/
			Vector3 stq;
			// Holy a7a, what is this cast? - Andrew-2E128
			generator->Compute_Texture_Coordinate((*world_vertex_locs)[ParentVertexIndices.back()], &stq);
			TexCoords.push_back(Vector2(stq.X,stq.Y));

		}
	}

	newdecal.FaceCount = Polys.size() - newdecal.FaceStartIndex;
	newdecal.VertexCount = ParentVertexIndices.size() - newdecal.VertexStartIndex;
	Decals.push_back(newdecal);

	material->Release_Ref();

	/*
	** tell the generator that we added a MeshDecal
	*/
	generator->Add_Mesh(Parent);

#ifdef WWDEBUG	
	/*
	** Some paranoid debug code: ensure all tris have valid vertex indices
	*/
	const size_t vert_count = ParentVertexIndices.size();
	for (const auto& poly : Polys)
	{
		WWASSERT (poly.I < vert_count);
		WWASSERT (poly.I >= 0);
		WWASSERT (poly.J < vert_count);
		WWASSERT (poly.J >= 0);
		WWASSERT (poly.K < vert_count);
		WWASSERT (poly.K >= 0);
	}
#endif
	
	// WWDEBUG_SAY(("Decal mesh now has: %d polys\r\n",Polys.Count()));
	return true;
}


/***********************************************************************************************
 * SkinDecalMeshClass::Delete_Decal -- Delete a decal                                         *
 *                                                                                             *
 * INPUT:                                                                                      *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   1/31/00    NH : Created.                                                                  *
 *=============================================================================================*/
bool SkinDecalMeshClass::Delete_Decal(uint32 id)
{
	/*
	** Find the MeshDecal which matches the given id
	*/
	size_t decal_index = -1;
	for (size_t i = 0;i < Decals.size(); i++) {
		if (Decals[i].DecalID == id) {
			decal_index = i;
			break;
		}
	}

	if (decal_index == -1) {
		return false;
	}

	DecalStruct * decal = &Decals[decal_index];

	/*
	** Remove all geometry used by this decal 
	*/
	Polys.erase(Polys.begin() + decal->FaceStartIndex, Polys.begin() + decal->FaceStartIndex + decal->FaceCount);
	ParentVertexIndices.erase(ParentVertexIndices.begin() + decal->VertexStartIndex, ParentVertexIndices.begin() + decal->VertexStartIndex + decal->VertexCount);
	TexCoords.erase(TexCoords.begin() + decal->VertexStartIndex, TexCoords.begin() + decal->VertexStartIndex + decal->VertexCount);

	/*
	** Re-index the remaining triangle vertex indices
	*/
	for (auto& poly : Polys)
	{
		if (poly.I > decal->VertexStartIndex) poly.I -= decal->VertexCount;
		if (poly.J > decal->VertexStartIndex) poly.J -= decal->VertexCount;
		if (poly.K > decal->VertexStartIndex) poly.K -= decal->VertexCount;
	}

	/*
	** Remove all materials used by this decal (remember to release refs!)
	*/
	for (int fi = decal->FaceStartIndex; fi < decal->FaceCount; fi++) {
		REF_PTR_RELEASE(Textures[fi]);
	}
	for (int vi=decal->VertexStartIndex; vi<decal->VertexCount; vi++) {
		REF_PTR_RELEASE(VertexMaterials[vi]);
	}
	Shaders.erase(Shaders.begin() + decal->FaceStartIndex, Shaders.begin() + decal->FaceStartIndex + decal->FaceCount);
	Textures.erase(Textures.begin() + decal->FaceStartIndex, Textures.begin() + decal->FaceStartIndex + decal->FaceCount);
	VertexMaterials.erase(VertexMaterials.begin() + decal->VertexStartIndex, VertexMaterials.begin() + decal->VertexStartIndex + decal->VertexCount);

	/*
	** Remove MeshDecal and refresh all other decal indices
	*/
	for (size_t di = decal_index + 1; di < Decals.size(); di++)
	{
		Decals[di].FaceStartIndex -= decal->FaceCount;
		Decals[di].VertexStartIndex -= decal->VertexCount;
	}
	Decals.erase(Decals.begin() + decal_index);

#ifdef WWDEBUG	
	/*
	** Some paranoid debug code: ensure all tris have valid vertex indices
	*/
	const size_t vert_count = ParentVertexIndices.size();
	for (auto& poly : Polys)
	{
		WWASSERT (poly.I < vert_count);
		WWASSERT (poly.I >= 0);
		WWASSERT (poly.J < vert_count);
		WWASSERT (poly.J >= 0);
		WWASSERT (poly.K < vert_count);
		WWASSERT (poly.K >= 0);
	}
#endif

	return true;
}
