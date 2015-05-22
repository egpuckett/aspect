/*
  Copyright (C) 2011 - 2015 by the authors of the ASPECT code.

 This file is part of ASPECT.

 ASPECT is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2, or (at your option)
 any later version.

 ASPECT is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with ASPECT; see the file doc/COPYING.  If not see
 <http://www.gnu.org/licenses/>.
 */

#include <aspect/particle/generator/uniform_box.h>

#include <boost/random.hpp>
#include <deal.II/base/std_cxx11/array.h>
#include <deal.II/grid/grid_tools.h>


namespace aspect
{
  namespace Particle
  {
    namespace Generator
    {
      // Generate random uniform distribution of particles over entire simulation domain

          /**
           * Constructor.
           *
           * @param[in] The MPI communicator for synchronizing particle generation.
           */
        template <int dim>
        UniformBox<dim>::UniformBox() {}

          /**
           * TODO: Update comments
           * Generate a uniformly randomly distributed set of particles in the current triangulation.
           */
          // TODO: fix the particle system so it works even with processors assigned 0 cells
        template <int dim>
        void
        UniformBox<dim>::generate_particles(Particle::World<dim> &world,
                                            const double total_num_particles)
          {
           int localParticleCount, startID;

           localParticleCount = total_num_particles;

           //Since we generate each particle on each processor, the above is irrelevant
           startID = 0;

           uniformly_distributed_particles_in_subdomain(world, total_num_particles, 0);
          }


          /**
           * Generate a set of particles uniformly randomly distributed within the
           * specified triangulation. This is done using "roulette wheel" style
           * selection weighted by cell volume. We do cell-by-cell assignment of
           * particles because the decomposition of the mesh may result in a highly
           * non-rectangular local mesh which makes uniform particle distribution difficult.
           *
           * @param [in] world The particle world the particles will exist in
           * @param [in] num_particles The number of particles to generate in this subdomain
           * @param [in] start_id The starting ID to assign to generated particles
           */
        template <int dim>
          void
          UniformBox<dim>::uniformly_distributed_particles_in_subdomain (Particle::World<dim> &world,
                                                      const unsigned int num_particles,
                                                      const unsigned int start_id)
                                                      {
          unsigned int cur_id = start_id;

          const Tensor<1,dim> P_diff = P_max - P_min;
          const double totalDiff = P_diff[0] + P_diff[1] + P_diff[2];
          std_cxx11::array<double,dim> nParticles;
          std_cxx11::array<double,dim> Particle_separation;

          ///Amount of particles is the total amount of particles, divided by length of each axis
          for (unsigned int i = 0; i < dim; ++i)
            {
              nParticles[i] = round(num_particles * P_diff[i] / totalDiff);
              Particle_separation[i] = P_diff[i] / nParticles[i];
            }

          for (double x = P_min[0]; x < P_max[0]; x+= Particle_separation[0])
            {
              for (double y = P_min[1]; y < P_max[1]; y += Particle_separation[1])
                {
                  if (dim == 2)
                      generate_particle(world,Point<dim> (x,y),cur_id++);
                  if (dim == 3)
                    for (double z = P_min[2]; z < P_max[2]; z += Particle_separation[2])
                      generate_particle(world,Point<dim> (x,y,z),cur_id++);
                }
            }
                                                      }
        template <int dim>
        void
        UniformBox<dim>::generate_particle(Particle::World<dim> &world,
                                           const Point<dim> &position,
                                           const unsigned int id)
            {
              typename parallel::distributed::Triangulation<dim>::active_cell_iterator it =
                  (GridTools::find_active_cell_around_point<> (this->get_mapping(), this->get_triangulation(), position)).first;;

              if (it->is_locally_owned())
                {
                  //Only try to add the point if the cell it is in, is on this processor
                  BaseParticle<dim> new_particle(position, id);
                  world.add_particle(new_particle, std::make_pair(it->level(), it->index()));
                }
            }


        template <int dim>
        void
        UniformBox<dim>::declare_parameters (ParameterHandler &prm)
        {
          prm.enter_subsection("Postprocess");
          {
            prm.enter_subsection("Tracers");
            {
              prm.enter_subsection("Generators");
              {
                prm.declare_entry ("Minimal x", "0",
                                   Patterns::Double (),
                                   "Minimal x coordinate for the region of tracers.");
                prm.declare_entry ("Maximal x", "1",
                                   Patterns::Double (),
                                   "Maximal x coordinate for the region of tracers.");
                prm.declare_entry ("Minimal y", "0",
                                   Patterns::Double (),
                                   "Minimal y coordinate for the region of tracers.");
                prm.declare_entry ("Maximal y", "1",
                                   Patterns::Double (),
                                   "Maximal y coordinate for the region of tracers.");
                prm.declare_entry ("Minimal z", "0",
                                   Patterns::Double (),
                                   "Minimal z coordinate for the region of tracers.");
                prm.declare_entry ("Maximal z", "1",
                                   Patterns::Double (),
                                   "Maximal z coordinate for the region of tracers.");

              }
            }
            prm.leave_subsection();
          }
          prm.leave_subsection();
        }


        template <int dim>
        void
        UniformBox<dim>::parse_parameters (ParameterHandler &prm)
        {
          prm.enter_subsection("Postprocess");
          {
            prm.enter_subsection("Tracers");
            {
              prm.enter_subsection("Generators");
              {
                P_min(0) = prm.get_double ("Minimal x");
                P_max(0) = prm.get_double ("Maximal x");
                P_min(1) = prm.get_double ("Minimal y");
                P_max(1) = prm.get_double ("Maximal y");

                if (dim ==3)
                  {
                    P_min(2) = prm.get_double ("Minimal z");
                    P_max(2) = prm.get_double ("Maximal z");
                  }
              }
              prm.leave_subsection();
            }
            prm.leave_subsection();
          }
          prm.leave_subsection();
        }
      }
    }
  }


// explicit instantiations
namespace aspect
{
  namespace Particle
  {
    namespace Generator
    {
      ASPECT_REGISTER_PARTICLE_GENERATOR(UniformBox,
                                         "uniform box",
                                         "Generate a uniform distribution of particles"
                                         " over a rectangular domain in or or 3D.")
    }
  }
}
