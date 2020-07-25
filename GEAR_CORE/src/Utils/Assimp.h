#pragma once
#include "gear_core_common.h"
#include "Objects/Material.h"

namespace gear 
{
namespace assimp_loader
{
	struct Vertex
	{
		mars::Vec4 m_Vertex;
		mars::Vec2 m_TexCoord;
		float m_TexId;
		mars::Vec4 m_Normal;
		mars::Vec4 m_Colour;
	};
	struct Mesh
	{
		std::vector<Vertex>	m_Vertices;
		std::vector<uint32_t> m_Indices;
		Ref<objects::Material> m_Material = CreateRef<objects::Material>();
	};

	static std::vector<Mesh> result;
	static std::vector<Mesh> LoadModel(const std::string& filepath)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR: GEAR::AssimpLoader::LoadModel: " << importer.GetErrorString() << std::endl;
			return std::vector<Mesh>(0);
		}
		result.clear();
		ProcessNode(scene->mRootNode, scene);
		return result;
	}

	static void ProcessNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			result.push_back(ProcessMesh(mesh, scene));
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			ProcessNode(node->mChildren[i], scene);
		}
	}

	static Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene)
	{
		Mesh result;
		//Vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex = {};
			if(mesh->HasPositions())
				vertex.m_Vertex = mars::Vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
			if(mesh->HasTextureCoords(0))
				vertex.m_TexCoord = mars::Vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			if(mesh->HasNormals())
				vertex.m_Normal = mars::Vec4(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0.0f);
			if(mesh->HasVertexColors(0))
				vertex.m_Colour = mars::Vec4(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, mesh->mColors[0][i].a);
			
			vertex.m_TexId = 0.0f;
			result.m_Vertices.push_back(vertex);
		}

		//Indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				result.m_Indices.push_back(face.mIndices[j]);
		}

		//Materials
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		for (unsigned int i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			std::vector<std::string> filepaths = GetMaterialFilePath(material, (aiTextureType)i);
			if (!filepaths.empty())
			{
				objects::Material::TextureType type;
				for (auto& filepath : filepaths)
				{
					switch(i)
					{
					case aiTextureType::aiTextureType_NONE:
						type = objects::Material::TextureType::GEAR_TEXTURE_UNKNOWN; break;
					case aiTextureType::aiTextureType_DIFFUSE:
						type = objects::Material::TextureType::GEAR_TEXTURE_DIFFUSE; break;
					case aiTextureType::aiTextureType_SPECULAR:
						type = objects::Material::TextureType::GEAR_TEXTURE_SPECULAR; break;
					case aiTextureType::aiTextureType_AMBIENT:
						type = objects::Material::TextureType::GEAR_TEXTURE_AMBIENT; break;
					case aiTextureType::aiTextureType_EMISSIVE:
						type = objects::Material::TextureType::GEAR_TEXTURE_EMISSIVE; break;
					case aiTextureType::aiTextureType_HEIGHT:
						type = objects::Material::TextureType::GEAR_TEXTURE_HEIGHT; break;
					case aiTextureType::aiTextureType_NORMALS:
						type = objects::Material::TextureType::GEAR_TEXTURE_NORMAL; break;
					case aiTextureType::aiTextureType_SHININESS:
						type = objects::Material::TextureType::GEAR_TEXTURE_SMOOTHNESS; break;
					case aiTextureType::aiTextureType_OPACITY:
						type = objects::Material::TextureType::GEAR_TEXTURE_OPACITY; break;
					case aiTextureType::aiTextureType_LIGHTMAP:
						type = objects::Material::TextureType::GEAR_TEXTURE_AMBIENT_OCCLUSION; break;
					case aiTextureType::aiTextureType_REFLECTION:
						type = objects::Material::TextureType::GEAR_TEXTURE_REFLECTION; break;
					case aiTextureType::aiTextureType_UNKNOWN:
						type = objects::Material::TextureType::GEAR_TEXTURE_UNKNOWN; break;
					default:
						type = objects::Material::TextureType::GEAR_TEXTURE_UNKNOWN; break;
					}

					graphics::Texture::CreateInfo texCI;
					texCI.device;
					texCI.filepaths = { filepath };
					texCI.data = nullptr;
					texCI.size = 0;
					texCI.width = 0;
					texCI.height = 0 ;
					texCI.depth = 0;
					texCI.format = miru::crossplatform::Image::Format::B8G8R8A8_UNORM;
					texCI.type = miru::crossplatform::Image::Type::TYPE_2D;
					texCI.samples = miru::crossplatform::Image::SampleCountBit::SAMPLE_COUNT_1_BIT;
					Ref<graphics::Texture> tex = CreateRef<graphics::Texture>(&texCI);

					result.m_Material->AddTexture(std::move(tex), type);
				}
			}
		}
		AddMaterialProperties(material, result.m_Material);

		return result;
	}

	static std::vector<std::string> GetMaterialFilePath(aiMaterial* material, aiTextureType type)
	{
		std::vector<std::string> result;
		for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
		{

			aiString str;
			material->GetTexture(type, i, &str);
			std::string filepath = str.C_Str();

			bool skip = false;
			for(std::vector<std::string>::iterator it; it < result.end(); it++)
			{
				if (*it == filepath)
				{
					skip = true;
					break;
				}
			}
			if(!skip)
				result.push_back(filepath);
		}
		return result;
	}

	static void AddMaterialProperties(aiMaterial* aiMaterial, Ref<objects::Material> material)
	{
		aiString name;
		int twoSided;
		int shadingModel;
		int wireframe;
		int blendFunc;
		float opacity;
		float shininess;
		float reflectivity;
		float shininessStrength;
		float refractiveIndex;
		aiColor3D colourDiffuse;
		aiColor3D colourAmbient;
		aiColor3D colourSpecular;
		aiColor3D colourEmissive;
		aiColor3D colourTransparent;
		aiColor3D colourReflective;

		aiMaterial->Get(AI_MATKEY_NAME, name);
		aiMaterial->Get(AI_MATKEY_TWOSIDED, twoSided);
		aiMaterial->Get(AI_MATKEY_SHADING_MODEL, shadingModel);
		aiMaterial->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe);
		aiMaterial->Get(AI_MATKEY_BLEND_FUNC, blendFunc);
		aiMaterial->Get(AI_MATKEY_OPACITY, opacity);
		//aiMmaterial->Get(AI_MATKEY_BUMPSCALING, colour_diffuse);
		aiMaterial->Get(AI_MATKEY_SHININESS, shininess);
		aiMaterial->Get(AI_MATKEY_REFLECTIVITY, reflectivity);
		aiMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);
		aiMaterial->Get(AI_MATKEY_REFRACTI, refractiveIndex);
		aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, colourDiffuse);
		aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, colourAmbient);
		aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, colourSpecular);
		aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, colourEmissive);
		aiMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, colourTransparent);
		aiMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, colourReflective);
		//aiMaterial->Get(AI_MATKEY_GLOBAL_BACKGROUND_IMAGE, colour_diffuse);

		material->AddProperties(
			{name.C_Str(),
			twoSided,
			shadingModel,
			wireframe,
			blendFunc,
			opacity,
			shininess,
			reflectivity,
			shininessStrength,
			refractiveIndex,
			mars::Vec4(colourDiffuse.r, colourDiffuse.g, colourDiffuse.b, 1),
			mars::Vec4(colourAmbient.r, colourAmbient.g, colourAmbient.b, 1),
			mars::Vec4(colourSpecular.r, colourSpecular.g, colourSpecular.b, 1),
			mars::Vec4(colourEmissive.r, colourEmissive.g, colourEmissive.b, 1),
			mars::Vec4(colourTransparent.r, colourTransparent.g, colourTransparent.b, 1),
			mars::Vec4(colourReflective.r, colourReflective.g, colourReflective.b, 1)}
		);
	}
}
}