/*
 * Copyright (C) 2020 Open Source Robotics Foundation
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
#ifndef IGNITION_RENDERING_POINTCLOUDVISUAL_HH_
#define IGNITION_RENDERING_POINTCLOUDVISUAL_HH_

#include <ignition/common/Time.hh>
#include <ignition/math/Color.hh>
#include <ignition/math/Vector3.hh>
#include "ignition/rendering/config.hh"
#include "ignition/rendering/Visual.hh"
#include "ignition/rendering/Object.hh"
#include "ignition/rendering/RenderTypes.hh"
#include "ignition/rendering/Marker.hh"

namespace ignition
{
  namespace rendering
  {
    inline namespace IGNITION_RENDERING_VERSION_NAMESPACE {
    /** \class PointCloudVisual PointCloudVisual.hh \
     * ignition/rendering/PointCloudVisual
     * \brief A PointCloudVisual class. The visual appearance is based
     * on the type specified.
     */
    class IGNITION_RENDERING_VISIBLE PointCloudVisual : public virtual Visual
    {
      protected: PointCloudVisual();

      /// \brief Destructor
      public: virtual ~PointCloudVisual();

      /// \brief Clear the points from the visual
      public: virtual void ClearPoints() = 0;

      /// \brief Update the visual
      public: virtual void Update() = 0;

      /// \brief Set points to be visualised
      /// \param[in] _points Vector of points representing distance of the ray
      public: virtual void SetPoints(
                  const std::vector<math::Vector3d> &_points) = 0;

      /// \brief Get number of points in the point cloud
      /// \return The number of points in the point cloud
      public: virtual unsigned int PointCount() const = 0;

      /// \brief Get the points in the point cloud
      /// \return The points in the point cloud
      public: virtual std::vector<math::Vector3d> Points() const = 0;
    };
    }
  }
}
#endif