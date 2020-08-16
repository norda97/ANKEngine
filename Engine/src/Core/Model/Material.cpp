#include "pch.h"
#include "Material.h"

#include "Core/Model/Model.h"


Material::Material()
{
	this->matProps.albedo = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	this->matProps.roughness = 0.5f;
	this->matProps.metallicness = 0.0f;
	this->matProps.specular = 1.0f;
}

Material::Material(const Material& material) noexcept : Material()
{
	this->matProps = material.getProperties();
}

Material::Material(Material&& material) noexcept : Material()
{
	this->matProps = material.getProperties();
}

Material::~Material()
{
}

Material& Material::operator=(const Material& material) noexcept
{
	this->matProps = material.getProperties();
	return *this; 
}

const MaterialProperties& Material::getProperties() const
{
	return this->matProps;
}

void Material::setAlbedo(const Vector4& albedo)
{
	this->matProps.albedo = albedo;
}

Vector3 Material::getAlbedo()
{
	return this->matProps.albedo;
}

void Material::setRoughness(float roughness)
{
	this->matProps.roughness = max(min(roughness, 1.0f), 0.025f);
}

float Material::getRoughness()
{
	return this->matProps.roughness;
}


void Material::setMetallicness(float metallicness)
{
	this->matProps.metallicness = max(min(metallicness, 1.0f), 0.0f);
}

float Material::getMetallicness()
{
	return this->matProps.metallicness;
}


void Material::setSpecular(float specular)
{
	this->matProps.specular = max(min(specular, 1.0f), 0.0f);
}

float Material::getSpecular()
{
	return this->matProps.specular;
}