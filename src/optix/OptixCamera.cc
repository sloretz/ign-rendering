/*
 * Copyright (C) 2015 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "ignition/rendering/optix/OptixCamera.hh"
#include "ignition/rendering/optix/OptixIncludes.hh"
#include "ignition/rendering/optix/OptixRenderTarget.hh"
#include "ignition/rendering/optix/OptixScene.hh"

using namespace ignition;
using namespace rendering;

//////////////////////////////////////////////////

const std::string OptixCamera::PTX_BASE_NAME("OptixCamera");

const std::string OptixCamera::PTX_RENDER_FUNCTION("Render");

//////////////////////////////////////////////////
OptixCamera::OptixCamera() :
  optixRenderProgram(NULL),
  renderTexture(NULL),
  antiAliasing(1),
  cameraDirty(true),
  entryId(0)
{
}

//////////////////////////////////////////////////
OptixCamera::~OptixCamera()
{
}

//////////////////////////////////////////////////
PixelFormat OptixCamera::GetImageFormat() const
{
  // return PF_UNKNOWN;
  return PF_R8G8B8;
}

//////////////////////////////////////////////////
void OptixCamera::SetImageFormat(PixelFormat /*_format*/)
{
}

//////////////////////////////////////////////////
math::Angle OptixCamera::GetHFOV() const
{
  return this->xFieldOfView;
}

//////////////////////////////////////////////////
void OptixCamera::SetHFOV(const math::Angle &_angle)
{
  this->xFieldOfView = _angle;
  this->poseDirty = true;
}

//////////////////////////////////////////////////
double OptixCamera::GetAspectRatio() const
{
  return this->aspectRatio;
}

//////////////////////////////////////////////////
void OptixCamera::SetAspectRatio(double _ratio)
{
  this->aspectRatio = _ratio;
  this->poseDirty = true;
}

//////////////////////////////////////////////////
unsigned int OptixCamera::GetAntiAliasing() const
{
  return this->antiAliasing;
}

//////////////////////////////////////////////////
void OptixCamera::SetAntiAliasing(unsigned int _aa)
{
  this->antiAliasing = _aa;
  this->cameraDirty = true;
}

//////////////////////////////////////////////////
void OptixCamera::PreRender()
{
  BaseCamera::PreRender();
  this->WriteCameraToDevice();
}

//////////////////////////////////////////////////
void OptixCamera::Render()
{
  unsigned int width = this->GetImageWidth();
  unsigned int height = this->GetImageHeight();
  optix::Context optixContext = this->scene->GetOptixContext();
  optixContext->launch(this->entryId, width, height);
}

//////////////////////////////////////////////////
RenderTexturePtr OptixCamera::GetRenderTexture() const
{
  return this->renderTexture;
}

//////////////////////////////////////////////////
void OptixCamera::WriteCameraToDevice()
{
  if (this->cameraDirty)
  {
    this->WriteCameraToDeviceImpl();
    this->cameraDirty = false;
  }
}

//////////////////////////////////////////////////
void OptixCamera::WriteCameraToDeviceImpl()
{
  this->optixRenderProgram["aa"]->setInt(this->antiAliasing + 1);
}

//////////////////////////////////////////////////
void OptixCamera::WritePoseToDeviceImpl()
{
  BaseCamera::WritePoseToDeviceImpl();

  math::Pose3d worldPose = this->GetWorldPose();
  math::Vector3d pos = worldPose.Pos();
  math::Matrix3d rot(worldPose.Rot());

  float3 eye = make_float3(pos.X(), pos.Y(), pos.Z());
  float3   u = make_float3(-rot(0, 1), -rot(1, 1), -rot(2, 1));
  float3   v = make_float3(-rot(0, 2), -rot(1, 2), -rot(2, 2));
  float3   w = make_float3( rot(0, 0),  rot(1, 0),  rot(2, 0));

  // TODO: handle auto and manual aspect-ratio
  // v *= 1 / this->aspectRatio;
  v *= (float)this->GetImageHeight() / this->GetImageWidth();
  w *= 1 / (2 * tan(this->xFieldOfView.Radian() / 2));

  this->optixRenderProgram["eye"]->setFloat(eye);
  this->optixRenderProgram["u"]->setFloat(u);
  this->optixRenderProgram["v"]->setFloat(v);
  this->optixRenderProgram["w"]->setFloat(w);
}

//////////////////////////////////////////////////
void OptixCamera::Init()
{
  BaseCamera::Init();
  this->CreateRenderTexture();
  this->CreateRenderProgram();
  this->Reset();
}

//////////////////////////////////////////////////
void OptixCamera::CreateRenderTexture()
{
  RenderTexturePtr base = this->scene->CreateRenderTexture();
  this->renderTexture = boost::dynamic_pointer_cast<OptixRenderTexture>(base);
  this->renderTexture->SetFormat(PF_R8G8B8);
  this->SetAntiAliasing(1);
}

//////////////////////////////////////////////////
void OptixCamera::CreateRenderProgram()
{
  this->optixRenderProgram =
      this->scene->CreateOptixProgram(PTX_BASE_NAME, PTX_RENDER_FUNCTION);

  optix::Context optixContext = this->scene->GetOptixContext();

  optixContext->setRayGenerationProgram(this->entryId,
      this->optixRenderProgram);

  optix::Buffer optixBuffer = this->renderTexture->GetOptixBuffer();
  this->optixRenderProgram["buffer"]->setBuffer(optixBuffer);

  // TODO: clean up
  this->optixErrorProgram =
      this->scene->CreateOptixProgram("OptixErrorProgram", "Error");

  this->optixErrorProgram["buffer"]->setBuffer(optixBuffer);
  optixContext->setExceptionProgram(this->entryId, this->optixErrorProgram);
}