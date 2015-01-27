/*
  Copyright (C) 2011 - 2014 by the authors of the ASPECT code.

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


#ifndef __aspect__compositional_initial_conditions_ascii_data_h
#define __aspect__compositional_initial_conditions_ascii_data_h

#include <aspect/compositional_initial_conditions/interface.h>
#include <aspect/velocity_boundary_conditions/ascii_data.h>
#include <aspect/simulator_access.h>


namespace aspect
{
  namespace CompositionalInitialConditions
  {
    using namespace dealii;

    /**
     * A class that implements the prescribed compositional fields
     * determined from a AsciiData input file.
     *
     * @ingroup CompositionalInitialConditionsModels
     */
    template <int dim>
    class AsciiData : public Interface<dim>, public SimulatorAccess<dim>
    {
      public:
        /**
         * Empty Constructor.
         */
        AsciiData ();

        /**
         * Initialization function. This function is called once at the
         * beginning of the program. Checks preconditions.
         */
        virtual
        void
        initialize ();

        /**
         * Return the initial composition as a function of position. For the
         * current class, this function returns value from the text files.
         */
        double
        initial_composition (const Point<dim> &position,
                             const unsigned int n_comp) const;

        /**
         * Declare the parameters this class takes through input files.
         */
        static
        void
        declare_parameters (ParameterHandler &prm);

        /**
         * Read the parameters this class declares from the parameter file.
         */
        void
        parse_parameters (ParameterHandler &prm);

      private:
        /**
         * Directory in which the data files are present.
         */
        std::string data_directory;

        /**
         * Filename of data file.
         */
        std::string data_file_name;

        /**
         * Number of grid points in data file
         */
        std_cxx11::array<unsigned int,3> data_points;

        /**
         * Scale the data by a scalar factor.
         */
        double scale_factor;

        /**
         * Pointer to an object that reads and processes data we get from
         * text files.
         */
        std_cxx11::shared_ptr<VelocityBoundaryConditions::internal::AsciiDataLookup<dim,dim> > lookup;
    };
  }
}


#endif
