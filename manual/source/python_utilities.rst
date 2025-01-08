.. _python-utilities:

****************
Python Utilities
****************

Several python packages are provided to aid in the preparation and conversion
of models to and from BDSIM's GMAD format as well as handle output data nicely
to make plots. There are individual utilities for specific formats but all are
tied together with the `pybdsim` repository.

* pybdsim - conversion and building of input, data loading, analysed data plotting
* pymadx  - file loader for MAD-X TFS file format
* pymad8  - file loader for MAD8 file format
* pytransport - file loader for Transport file format

Installing Python Utilities
===========================

Installation is described in the setup section in :ref:`setup-python-utilities`.

But trivially, one can do :code:`pip install pybdsim` and this will install all of
the Python utilities. pybdsim will work without ROOT but if available, it will be
able to load the raw and histogram data files.

.. _python-utilities-documentation:

Documentation
=============

In the following sections is individual documentation for each python package.
These are also available when using iPython by typing '?' after any module, class
or function.

   * `<http://www.pp.rhul.ac.uk/bdsim/pybdsim/>`_
   * `<http://www.pp.rhul.ac.uk/bdsim/pymadx/>`_
   * `<http://www.pp.rhul.ac.uk/bdsim/pymad8/>`_
   * `<http://www.pp.rhul.ac.uk/bdsim/pytransport/>`_

.. _python-geometry-preparation:
     
Geometry Preparation
====================

For preparation of custom geometry, we have developed a Python geometry package
called `pyg4ometry`. This allows scripting of geometry creation in Python, visualisation
and overlap checking outside Geant4. The emitted GDML files can then be loaded in BDSIM.
It can also be used to composite STL, GDML, Python defined geometry, FLUKA geometry into one.
See:

 * `<https://pyg4ometry.readthedocs.io/en/stable/>`_
 * `<https://github.com/g4edge/pyg4ometry>`_

