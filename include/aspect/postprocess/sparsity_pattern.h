/*
  Copyright (C) 2011, 2012 by the authors of the ASPECT code.

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
/*  $Id$  */


#ifndef __aspect__postprocess_sparsity_pattern_h
#define __aspect__postprocess_sparsity_pattern_h

#include <aspect/postprocess/interface.h>
#include <aspect/simulator.h>


namespace aspect
{
  namespace Postprocess
  {

    /**
     * A postprocessor that computes the sparsity pattern for the finite element matrix.
     *
     * @ingroup Postprocessing
     */
    template <int dim>
    class SparsityPattern : public Interface<dim>, public ::aspect::SimulatorAccess<dim>
    {
      public:
        /**
         * Constructor
         **/
        SparsityPattern ();

        /**
         * Generate output for the sparsity pattern for the matrix.
         **/
        virtual
        std::pair<std::string,std::string>
        execute (TableHandler &statistics);

        /**
         * Declare the parameters this class takes through input files.
         */
        static
        void
        declare_parameters (ParameterHandler &prm);

        /**
         * Read the parameters this class declares from the parameter
         * file.
         */
        virtual
        void
        parse_parameters (ParameterHandler &prm);

      private:
        /**
         * Interval between the generation of output. This
         * parameter is read from the input file and consequently is not part
         * of the state that needs to be saved and restored.
        *
        * For technical reasons, this value is stored as given in the
        * input file and upon use is either interpreted as seconds or
        * years, depending on how the global flag in the input parameter
        * file is set.
         */
        double output_interval;

        /**
         * A time (in years) after which the next time step should produce
         * graphical output again.
         */
        double next_output_time;

        /**
         * Compute the next output time from the current one. In
         * the simplest case, this is simply the previous
         * next output time plus the interval, but in general
         * we'd like to ensure that it is larger than the current
         * time to avoid falling behind with next_output_time and
         * having to catch up once the time step becomes larger.
         */
        void set_next_output_time (const double current_time);
    };
  }
}


#endif