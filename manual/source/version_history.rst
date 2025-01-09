Planned Development
===================

Below is a brief list of planned developments for the next version. Please get in touch
if you'd like to give us feedback or help in the development.  See :ref:`support-section`.

* Multiple beam line tracking.
* Restructure code into more C++ libraries rather than just analysis and 'bdsim'.
* Interpolated aperture shapes between any two shapes.
* Tapered aperture for all elements.
* Beam pipe sections to fill gaps between changes in aperture.
* Any aperture shape can be used for both the inside and the outside of a collimator.


v1.8.0 - 2024 / XX / XX
=======================

The BDSIM source code has moved to Github and is available here: https://github.com/bdsim-collaboration/bdsim

This move was done due to better provision of testing resources and interfaces. All issues
should now be launched here. A more modern approach of pull-requests is being used and
more publicly accessible and complete continuous integration is now used. This was done
to maintain the expected high quality of the code.

* For models with acceleration, the rigidity and synchronous time are now calculated
  along the beamline and pre-calculated **scaling factors are no longer needed**.


New Features
------------

**Fields**

* The `rf` beamline element now has the parameter :code:`cavityFieldType` to specify which
  field model to use rather than specifying :code:`fieldVacuum` and a corresponding field
  definition.
* The option :code:`cavityFieldType` may be used to set the default field model for all `rf`
  elements.
* The "rfcavity" field is now "rfpillbox".


**General**

* :code:`autoColour=1` now works for all collimators and target elements. If turned on, the
  colour of the element in the visualiser will be given by the material.

**Physics**

* New :code:`ionisation` modular physics list for only the ionisation process for the most
  common particles.



New Options
-----------

.. tabularcolumns:: |p{0.30\textwidth}|p{0.70\textwidth}|

+-------------------------------------+-------------------------------------------------------+
| **Option**                          | **Function**                                          |
+=====================================+=======================================================+
| cavityFieldType                     | Default cavity field type ('constantinz', 'pillbox')  |
|                                     | to use for all rf elements unless otherwise specified.|
+-------------------------------------+-------------------------------------------------------+
| integrateKineticEnergyAlongBeamline | Integrate changes to the nominal beam energy along    |
|                                     | the beamline such as from accelerator and adjust      |
|                                     | the design rigidity for normalised fields             |
|                                     | accordingly.                                          |
+-------------------------------------+-------------------------------------------------------+

General Updates
---------------

* The interface for custom components has changed due to the new beamline integral class and object.
  The example has been updated accordingly.
* Internally, beamline elements are now cached based on both their name (basic reuse of components)
  but also the nominal rigidity at that point in the beamline. This is because if, say, a quadrupole
  is used later in the beamline after acceleration with the same `k1`, the actual field gradient
  is different and so the component must be uniquely constructed to have a different field.
* The time coordinate is now loaded and applied to each particle when loading a bdsim output
  sampler as a distribution.

Bug Fixes
---------

* Fix rebdsim's Spectra command preparing the wrong variables when used on a cylindrical
  or spherical sampler where the variable is "totalEnergy" and not "energy".
* Fix a bug where rebdsim would crash if a Spectra command was used on a cylindrical or
  spherical sampler. This was caused by loading the data into the wrong class.
* The pill-box field was fixed where it should have no `z` dependence whereas it did previously.


Output Changes
--------------

* The synchronous time at the middle of an element (:code:`midT`); the momentum at the
  beginning of an element (:code:`staP`); and the kinetic energy at the beginning of
  an element (:code:`staEk`) have all been added to the model tree in the output as
  calculated by BDSIM as it now integrates the time and acceleration / decceleration
  along the beamline.


Output Class Versions
---------------------

* Data Version 10.

+-----------------------------------+-------------+-----------------+-----------------+
| **Class**                         | **Changed** | **Old Version** | **New Version** |
+===================================+=============+=================+=================+
| BDSOutputROOTEventAperture        | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventBeam            | N           | 6               | 6               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCavityInfo      | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimator      | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimatorInfo  | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCoords          | N           | 3               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHeader          | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHistograms      | N           | 4               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventInfo            | N           | 7               | 7               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLoss            | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLossWorld       | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventModel           | Y           | 6               | 7               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventOptions         | N           | 8               | 8               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventRunInfo         | N           | 3               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSampler         | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSamplerC        | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSamplerS        | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectory      | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectoryPoint | N           | 6               | 6               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTParticleData         | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+


Utilities
---------

These are no longer included directly with BDSIM but are available through pip. At the time
of writing, the corresponding versions of each utility are:

* pybdsim v3.3.2
* pymadx v2.0.1
* pymad8 v2.0.1
* pytransport v2.0.1


V1.7.7 - 2024 / 01 / 29
=======================

Reminder: the Python utilities (pybdsim, pymadx, pymad8, pytransport) are published and
updated through pip and are not distributioned with BDSIM itself.

New Features
------------

* Introduced X-ray reflection from Geant4 11.2 onwards.

**New beam command options:**

.. tabularcolumns:: |p{0.30\textwidth}|p{0.70\textwidth}|

+-------------------------------------+------------------------------------------------------+
| **Option**                          | **Function**                                         |
+=====================================+======================================================+
| eventGeneratorWarnSkippedParticles  | 1 (true) by default. Print a small warning for each  |
|                                     | event if any particles loaded were skipped or there  |
|                                     | were none suitable at all and the event was skipped. |
+-------------------------------------+------------------------------------------------------+

**New options:**

.. tabularcolumns:: |p{0.30\textwidth}|p{0.70\textwidth}|

+-------------------------------------+-------------------------------------------------------+
| **Option**                          | **Function**                                          |
+=====================================+=======================================================+
| visVerbosity                        | (0-5 inclusive) the verbosity level passed into the   |
|                                     | Geant4 visualisation system. 0 is the default.        |
+-------------------------------------+-------------------------------------------------------+
| xrayAllSurfaceRoughness             | The length scale of roughness features for the X-ray  |
|                                     | reflection model (from the `xray_reflection` physics  |
|                                     | modular list). Default 0, units metres. A typical     |
|                                     | value would be 5 nm. This applies to all surfaces.    |
+-------------------------------------+-------------------------------------------------------+


General Updates
---------------

* Update copyright year throughout code.
* Updated format for :code:`makematerialfile` program for exporting NIST information to pyg4ometry.
* Improved error messages for bad scorer mesh definition.
* Improved description in manual of physics list recommendation.
* Reduced printout for the visualisation.


Bug Fixes
---------

Hot-fix for issue #377. A tracking issue appeared in thin elements due to a too small maximum value for the
relative error, epsilonStep, resulting in incorrect kicks being applied. This occurred only when BDSIM is compiled
against versions of Geant4 11.0 onwards. The maximum value is now set separately for thick and thin volumes.

* Fix for C++20 compilation for ROOT installations that now have C++20 on LCG.
* Fix for parser rounding of double values put into integer parameters.
* Fix overwriting of ROOT_INCLUDE_PATH environmental variable if it already existed in :code:`bdsim.sh`.
* Fix obeying the Geant4 default visualiser for 11.2 onwards.


In the parser, it is possible to do some simple calculations and use these variables
as input to parameters. For example, calculating the number of bins in a mesh. When
the parameter type was an integer, but a floating point number was given (perhaps from
the calculation or from writing a ".0" after a number), the floating point double would
be put into an integer and you may get unexpected rounding errors. e.g. 29.999999999997 becomes 29
instead of the 30 that was expected. This has been fixed by rounding to the nearest integer
only when using a double into a integer parameter.


Output Changes
--------------

Only the options have changed which are stored to file also. These are backwards compatible
and no issues are expected with loading older data.


Output Class Versions
---------------------

* Data Version 9.

+-----------------------------------+-------------+-----------------+-----------------+
| **Class**                         | **Changed** | **Old Version** | **New Version** |
+===================================+=============+=================+=================+
| BDSOutputROOTEventAperture        | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventBeam            | N           | 6               | 6               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCavityInfo      | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimator      | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimatorInfo  | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCoords          | N           | 3               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHeader          | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHistograms      | N           | 4               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventInfo            | N           | 7               | 7               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLoss            | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLossWorld       | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventModel           | N           | 6               | 6               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventOptions         | Y           | 8               | 7               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventRunInfo         | N           | 3               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSampler         | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSamplerC        | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSamplerS        | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectory      | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectoryPoint | N           | 6               | 6               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTParticleData         | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+



V1.7.6 - 2023 / 10 / 18
=======================

Hot-fix for muon splitting. When muon splitting occurred it turned on a flag in the G4VParticleChange
object belonging to the physics process in Geant4 as required to give each particle unique weights
(as non-muon secondaries are not split). However, most processes in Geant4 reuse the same object and
do not reset this with each initialisation of a track so it remains in place. Also, the same G4Decay
process object is registered to different particle definitions. When decay happens for another particle
after this (for a particle that is not in the muon splitting), the flag results in the weights not being
transferred to the new secondaries.

In short, particles that were not in the list (pi+, pi-, e+, kaon+, kaon-, kaon0L) that had an incoming
weight from other biasing would have their weights reset to 1, only after splitting had occurred once
in that run. And for every subsequent event.

* It is not required to set :code:`beam, distrFileLoop=1` if :code:`beam, distrFileLoopNTimes` is set
  to a value greater than 1 for any file-based input distributions.


v1.7.5 - 2023 / 10 / 03
=======================

General Updates
---------------

* The ability to purposively override the (good) default maximum step length in a
  field map is provided in a field definition. Normally, the maximum step length is
  limited to 1x the minimum grid spacing of a field map in any dimension. Larger
  steps result in the field only being evaluated on that length scale and therefore
  giving a possibly wrong numerical integration of the field. However, in specific
  high-energy cases, it is a useful optimisation to increase this length. This must
  be used with knowledge and caution though.

Bug Fixes
---------

* Weights for primaries were not loaded from file when using a :code:`bdsimsampler` distribution.
  Previously, they were all weight 1 by default. This affects any second stage simulation where
  biasing was used in the first stage.
* CMake fix for HepMC3 for versions greater than 3.1.1.
* Fix :code:`geant4Version` in the header output as it didn't contain the patch number
  as Geant4's string for this is a little inconsistent.
* :code:`BDSOutputROOTEventTrajectory` copy constructor did not copy the `mass` variable.



V1.7.4 - 2023 / 08 / 25
=======================

New Features
------------

* Spectra by momentum: :code:`SpectraMomentum` in rebdsim.


General Updates
---------------

* Fix manual description of 3D histograms in rebdsim. Should be :code:`z:y:x` for 3D histogram
  variables. :code:`y:x` for 2D, and :code:`x` for 1D histograms.


Bug Fixes
---------

* Fix S coordinate of primaries in the output if a negative :code:`option, beamlineS`
  was used. It would previously always be 0.
* Fix print out in terminal of how many events have been completed when using a file-based
  beam distribution. It would always be each event instead of the usual 10%, which may have
  slowed down simulations or inflated log files.


V1.7.3 - 2023 / 08 / 11
=======================

* Hotfix - undo recent optimisation for histograms as it accidentally affected the mean
  in non-simple (i.e. per-entry average) histograms.


V1.7.2 - 2023 / 08 / 11
=======================

General Updates
---------------

* Determine extents of any container solid loaded from an external geometry file.

Bug Fixes
---------

**Installation**

* Fix compilation when BDSIM is compiled with GDML on but the Geant4 used does not
  have GDML compiled into it. This would result in a compilation error rather than
  a CMake error at configuration time.

**Geometry**

* A placement where the outermost solid was an extruded solid would cause the extents
  not to be determined properly and therefore the maximum step size to be set to 1 micron,
  which would result in very slow running events. Fixed by automatically determining the
  size of any potential solid given from externally loaded geometry.
* Generic BLM shapes now have consistent user limits for tracking applied as other volumes.
* Do not allocate a G4UserLimits object for every placement that wasn't used.

**Visualisation**

* `shield` component now obeys `colour` property correctly.


V1.7.1 - 2023 / 07 / 20
=======================

* Fix NANs appearing in merged histograms from rebdsimCombine where histograms were empty.
* Fix wrong number of entries in per-entry histograms (e.g. per-event histograms)
  from rebdsim.
* Fix crash from Geant4 GDML writer when exporting GDML geometry from BDSIM where
  a directory is included in the destination file path but it does not exist.


V1.7.0 - 2023 / 07 / 11
=======================

* The input parser will now reject any duplicate object names (e.g. a field with the same name),
  whereas it didn't before. In the past, multiple objects would be created ignoring their name.
  However, after the input is loaded, BDSIM itself may look through the objects for one matching
  a name. In this case, the one it finds may be ambiguous or unexpected. The code was revised to
  purposively protect against this. This was always the case with beam line elements, but now it
  is also the case with all objects defined in the parser.
* The input parser will now reject any variable names that are the same as an option name as
  this is a common mistake where we put a semi-colon before another option then it has no effect.
* GGMAD Geometry format is now deprecated. This was not maintained for a long time and with
  pyg4ometry and GDML we support much better geometry. The code is old and hard to maintain
  and really needs to be rewritten. The functionality was broken in making BDSIM compatible
  with all the changes to string handling in Geant4 V11.
* New executable options :code:`--reference` and :code:`--citation` to display the citation
  in Bibtex syntax to cite BDSIM easily.
* The default yoke fields have changed and are on average stronger (and more correct). See below.
* :code:`gradient` in the :code:`rf` component has the units of **V/m** and not MV/m as was
  written in the manual. Any rf component in an existing model that is defined with a :code:`gradient` but
  without units should be updated to include units of MV/m. The documentation has been fixed and is correct
  and consistent. The units for :code:`E` have also been clarified as volts and that this voltage is assumed
  across the length of the element :code:`l`.
* File looping is no longer the default for filed-based input distributions. The default behaviour
  is now **to match the length** of the input distribution file.


New Features
------------

(topics alphabetically)

**Analysis**

* New Spectra command for rebdsim to make very flexible sets of spectra automatically. See
  :ref:`spectra-definition` for more information.
* rebdsim will now default to `<inputfilename>_ana.root` if no outputfile name is specified.
* Similarly, rebdsimHistoMerge will default to `<inputfilename>_histos.root`; rebdsimOptics to
  `<intputfilename>_optics.root` and bdskim to `<inputfilename>_skimmed.root`.
* bdsimCombine will now produce an extra tree in the output called "EventCombineInfo" that
  contains an index to which file the event came from.
* New :code:`VerboseSpectra` option to print out the full definitions of all spectra histograms
  as automatically generated by rebdsim.
* New :code:`total` keyword for spectra definition to get all particles in one histogram for
  the usual `total` envelope. The word "total" is used to distinguish it from "all" which means
  a unique histogram for each PDG ID found in the data.

**Beam**

* New bunches feature allows offset in time for different bunches at a given repetition rate
  or period with a certain number of events at a fixed bunch index generated. See :ref:`beam-bunches`.
* The `square` bunch distribution can now have an uncorrelated `Z` distribution with time by
  explicitly specifying `envelopeZ`. If unspecified, the original behaviour remains.
* New bunch distribution type `halosigma` that samples a flat halo distribution
  flat in terms of sigma. This is useful for re-weighting distributions based on
  the particle's distance from the core in terms of sigma.
* The `halo` distribution now has an outer position cut in both X and Y axes, specified
  by `haloXCutOuter` and `haloYCutOuter` respectively. Similar inner and outer cuts of the X and Y
  momentum are also now possible, specified by same options as the position cuts but with a `p`
  after the axis, e.g `haloXpCutOuter`.
* The radius of the transverse momentum distribution of a circular beam no longer has to be finite.
  This is useful for generation of an idealised pencil beam.
* All neutrinos can be used as beam particles now (useful for visualisation of neutrino lines).
* The `eventgenerator` and `bdsimsampler` distributions now have `eventGeneratorNEventsSkip`
  in the beam command to allow skipping into the file.
* Consistency between features between `eventgenerator` and `bdsimsampler` distribution.
* A new executable option `--distrFileLoopNTimes=<N>` allows you to repeat an input file `N`
  times while matching the length to replay the same input coordinates from a distribution
  file with different physics easily.

**Components**

* A new `ct` keyword has been implemented to allow the conversion of DICOM CT images into
  voxelized geometries.
* New `rfx` and `rfy` components for transverse RF fields.
* New `target` beam line component. We could always create a block of material with a closed
  `rcol` but this is more intuitive.

**Fields**

* New ability to use any "pure" field (i.e. one from equations inside BDSIM) as a field
  and attach it to placements, or beam line geometry, as well as query it to generate
  an external field map.
* New ability to query a 3D model for the field and export a field map.
* New program bdsinterpolator to interpolate a field map and export it without
  any handling by Geant4.
* New field drawing facility in the visualiser to draw query objects.
* Field map reflections have been introduced allowing symmetry to be exploited.
  See :ref:`fields-transforms`.
* "linearmag" interpolation added.
* New ability to arbitrarily scale the yoke fields.
* New `modulator` object to modulate RF components (see :ref:`field-modulators`).
* `reflectxydipole` added flip in Fz for y < 0.

**General**

* New :code:`--versionGit` executable option to get the git SHA1 code as well as the version number.
* New :code:`--E0=number`, :code:`--Ek0=number`, and :code:`--P0=number` executable options are
  introduced to permit overriding the energy of the beam.
* New executable option :code:`--geant4PhysicsMacroFileName` to control the physics macro from the
  command line. Useful when BDSIM is executed from a different directory from the main GMAD input
  file and with a relatively complex model.
* New Docker script in :code:`bdsim/building/docker/build-centos-bdsim.sh` and updated
  instructions on how to run Docker. This is a container system where a complete
  environment build on Centos7 will be built locally and works on Mac, Linux, Windows. It
  typically takes about 6Gb of space and is a great alternative to a virtual machine. An
  XWindows server is required for the visualiser. See :ref:`docker`.
* New materials (Inermet170, Inermet176, Inermet180, Copper-Diamond, MoGr).
* Nicer visualisation colours for charged particles. Green for neutrals is by default now at
  20% opacity as there are usually so many gammas.
* New units: `mV`, `GV`, `nrad`, `THz`.
* New :code:`verboseSensitivity` option to print out the sensitive detector by name at every
  level of the hierarhcy.

**Geometry**

* An :code:`element` beam line component now works with :code:`angle` as a parameter and
  the sign convention has been changed to match the bends as per MADX where a positive angle
  corresponds to a displacement in negative `x` in a right handed coordinate system with the
  beamline built along `z`. Drifts on either side will now match the element if `e1` and `e2`
  (traditionally pole-face angles) are given for the element.
* The length :code:`l` for :code:`element` is now treated as the chord length rather than the
  arc length. This has no effect for straight components, but makes it easier to use angled
  elements.
* When loading geometry (e.g. a GDML file) to be used as a placement, you can now remove the
  outermost volume (e.g. the 'world' of that file) and place all the contents in the BDSIM
  world with the compound transforms: relative to the former outermost logical volume and also
  the placements transform in the world. This works by making the outer volume into a G4AssemblyVolume.
* Ability to inspect G4EllipticalTube for extents as a container volume of imported GDML geometry
  as required for NA62.
* Ability to read GDML auxiliary information for the tag "colour" to provide colour information
  in the GDML file.
* Beam pipe aperture may now be defined by a series of x,y points in a text file for an
  arbitrary shaped beam pipe. This may also be used as the default one.
* New :code:`rhombus` aperture type.

**Physics**

* New option :code:`restoreFTPFDiffractionForAGreater10` to turn back on nucleon diffraction
  in hadronic physics for Geant4 v11.1 onwards. See :ref:`physics-proton-diffraction`. This
  is **on** by default.
* New muon-splitting biasing scheme.
* New "radioactivation" physics list.
* New "gamma_to_mumu" physics list.
* New "annihi_to_mumu" physics list.
* New "muon_inelastic" physics list.
* New option for excluding certain particles from cuts, e.g. exclude muons from the
  minimumKineticEnergy option. See :code:`particlesToExcludeFromCuts` in :ref:`options-tracking`.

**Sensitivity & Output**

* Add an option :code:`uprootCompatible` to read the output file with uproot. If set to 1,
  it corresponds to have :code:`samplersSplitLevel=1` and :code:`modelSplitLevel=2`.
* Samplers now have the parameter :code:`partID={11,-11}`, which for example can be used
  to filter only which particles are recorded in a given sampler. See :ref:`sampler-filtering`.
  This also applies to sampler placements.
* New **spherical** and **cylindrical** samplers.  See :ref:`sampler-types-and-shapes`.
* The :code:`csample` command now works correctly and has been re-implemented for all beamline
  components.
* A sampler in a BDSIM ROOT output file can now be used as an input beam distribution for
  another simulation.  See :ref:`beam-bdsimsampler`.
* Solenoid sheet / cylinder field has been added and is used by default on the solenoid yoke geometry.
* Scoring of the differential flux (3D mesh + energy spectrum per cell) following either a linear,
  logarithmic or user-defined energy axis scale (requires Boost).
* New scorer type: cellflux4d.
* New type of scorermesh geometry: cylindrical.
* Materials are now stored for each trajectory step point (optionally) as described
  by an integer ID.
* New trajectory filter option to store only secondary particles. Can be used in combination
  with particle type to select only secondary particles that may be the same type of particle
  as the primary particle. The option is :code:`storeTrajectorySecondaryParticles`. The bitset
  for which filter was passed has been accordingly extended from 9 bits to 10 bits and the new
  filter is the the last one. This is reflected in the file header that stores the names of the
  filters.
* New options :code:`storeElossWorldIntegral` and :code:`storeElossworldContentsIntegral` that can
  be used alone to store only the single total energy deposition (including weights) in the world and
  world contents (in case of an externally provided world volume) without storing all the individual
  hits that would use a lot of disk space.
* :code:`storeSamplerKineticEnergy` is now on by default.


General Updates
---------------

* The `userfile` distribution now doesn't count comment lines for `nlinesSkip` - only valid data lines.
* When using the minimum kinetic energy option, tracks are now stopped in the stacking action
  rather than being allowed to be tracked for a single step. This should vastly improve the
  speed of some events with large numbers of tracks.
* The minimum kinetic energy option is printed out if used now as it is important.
* The default yoke fields have been revised. The equation for the field is the same, but the
  normalisation to the pure vacuum field at the pole-tip has been fixed and improved. This
  leads to the removal of very high peak values close to the hypothetical current sources
  between poles and also generally increases the average field magnitude in the yoke. This makes
  a smooth transition from the vacuum field to the yoke field and is more correct. Specifically,
  the contribution from each current source is evaluated half way between each current source
  for the purpose of normalisation. The new option :code:`useOldMultipoleOuterFields=1` is
  available to regain the old behaviour. This will be removed in the next version beyond this one.
* Compatibility with Geant4 V11.
* Optional dependency on Boost libraries (at least V1.71.0) for 4D histograms.
* The option :code:`scintYieldFactor` has no effect from Geant4 V11 onwards.
* The executable option :code:`--geant4Macro` (for a post-visualisation macro)
  has been renamed to :code:`--geant4MacroFileName` to be the same as the option in
  the input GMAD file. The old one is still accepted for backwards compatibility.
* The userfile distribution will tolerate `!` to denote a comment line to match GMAD syntax now.
  It will also tolerate any white-space before either `#` or `!` to mark a comment line,
  whereas previously it would only identify a comment if the very first character
  of the line was `#`.
* BDSGeometryComponent class refactored to permit a G4AssemblyVolume as the container
  for a piece of geometry. It's in addition to a logical volume.
* A `dump` element may now be specified without a length and will by default be 1 mm long.
* The visualiser command :code:`/bds/beamline/goto` now accepts an optional integer as a second
  argument to specify the instance of a beam line element in the line to go to. i.e. if the same
  beam line element is reused, you can select an individual one to go to.
* Tolerate "electron", "positron" and "photon" for beam particle names and substitute in the
  Geant4 names (e.g. "e-").
* Print out extent of loaded world when using an external geometry file.
* **EMD** physics has a minimum applicable kinetic energy of 1 MeV to prevent crashes in Geant4.
* Optional executable argument added to ptc2bdsim to control ROOT split-level of sampler branches. Same
  functionality as the BDSIM option :code:`samplersSplitLevel`.
* The green colour for collimators and the new target component has been adjusted very slightly
  to be a little brighter.
* Parser error messages for samplers have been improved to give line numbers and exact
  strings in quotes.
* Samplers, sampler placements and their parallel world have been change to have a nullptr (no)
  material. The parallel world material should not make a difference for the setup in BDSIM, but
  now it is explicitly forbidden from having any effect by it being nullptr.
* The material print out (:code:`bdsim --materials`) now includes aliases.
* When using `autoScale` for a field map attached to the yoke of a magnet, the calculated scaling
  factor is now always print out for feedback.
* The visualiser command `/bds/beamline/list` now prints the S middle coordinate in metres.

Bug Fixes
---------

(topics alphabetically)

**Analysis**

* rebdsim will now explicitly exit if a duplicate histogram name is detected whereas it didn't before.
* If an electron was used as the beam particle, the mass might not be set correctly for optics analysis
  (only) resulting in wrong results for sub-relativistic electron optics.
* Fix spectra in rebdsim when used with a selection. The selection would be built up wrongly and wouldn't
  work in the past.
* Fix warning when using sampler data in analysis in Python: ::

    input_line_154:2:36: warning: instantiation of variable 'BDSOutputROOTEventSampler<float>::particleTable' required here, but no
      definition is available [-Wundefined-var-template]
    BDSOutputROOTEventSampler<float>::particleTable;
                                   ^
    .../bdsim-develop-install/bin/../include/bdsim/BDSOutputROOTEventSampler.hh:135:37: note: forward declaration of template entity is here
    static BDSOutputROOTParticleData* particleTable;
                                    ^
    input_line_154:2:36: note: add an explicit instantiation declaration to suppress this warning if
    'BDSOutputROOTEventSampler<float>::particleTable' is explicitly instantiated in another
    translation unit
    BDSOutputROOTEventSampler<float>::particleTable;

* Fix print out of event numbers in rebdsim being analysed when :code:`EventStart` and :code:`EventEnd` are
  specified. Also the print out rate given the possibly reduced number of events.
* Fixed possible NANs in the merged output histograms if histograms were supplied with 0 entries.

**Beam**

* The `userfile` distribution now doesn't count comment lines for `nlinesSkip` - only valid data lines.
* Fix infinite looping in the `userfile` distribution if `nlinesIgnore` or `nlinesSkip` were longer
  than the number of lines in the file.
* Fixed generation of circular beam distribution type. The beam previously was circular but was non-uniform with a strong
  peak at the centre. The distribution is now uniform in x, y, xp & yp.
* Fixed generation of ring beam distribution type. Similarly to the circular distribution, the beam had a higher density
  of particles towards the ring's inner radius. The distribution is now uniform in x & y.
* Fixed recreation when using a `ptc` distribution as the file wouldn't advance to the correct entry.
* The `square` distribution now has uncorrelated `z` and `t`. You can restore the old behaviour with
  the new beam parameter: :code:`beam, zFromT=1;` that only works for this distribution.
* General fix for use of static interfaces of the random number generator through instances. Uniform
  flat distributions were constructed but always the static interface to the static generator was
  used making it confusing. This has been fixed to explicitly use the static interface and not
  construct and interface. The exact same results are produced reproducibly.
* Fixed "antiproton" as a beam particle. Should really be "anti_proton" for Geant4 to find
  the particle definition. Manual updated accordingly.
* Fixed "kaon0L" as a beam particle. Also allow "kaon0S" and "kaon0".
* Fixed beam offset with S when using negative `beamlineS` option for generally offsetting the
  S coordinate (as a variable in all data).

**Biasing**

* Fixed huge amount of print out for bias objects attached to a whole beam line. Now, bias
  objects are only constructed internally for a unique combination of biases from the input.
  Less print out and (marginally) lower memory usage.

**Fields**

* Fix time units of BDSIM-format field maps that included time in any dimension. Previously,
  they were always in 0.1s instead of 1s.
* Fix field maps being wrong if a GDML file was used multiple times with different fields.
* Fix BDSIM-format field map loading with :code:`loopOrder> tzyx` in the header. It was not
  loaded correctly before. Also, there are corresponding fixes in the pybdsim package.
* Fix lack of yoke fields for rbends.
* Fix lack of yoke fields and also orientation of fields in (thick) hkickers and vkicker magnets.
* Fix LHC 'other' beam pipe field which was not offset to the correct position. Mostly a fault for
  quadrupoles where the field appeared in effect as a distorted dipole field (i.e. very off-axis quadrupole field).
* Fix field interpolation manual figures. Z component was transposed.
* Fixed example field map generation scripts to not use tar as we don't support loading
  of tar.gz (only gzipped or uncompressed) files (historical hangover).
* Fixed field map interpolation and plotting scripts as well as make use of improvements
  in pybdsim.
* Fix a bug in field map loading where a space was before the "!" character the columns
  wouldn't be parsed correctly.
* Fix BDSIM field map format :code:`loopOrder` documentation. The variable can be either `xyzt` or `tzyx`.
* The quadrupole field in an sbend or rbend with a k1 value specified was a factor of 1e6 too
  low due to the placement of units. The integrator for tracking (which ignores the field) was
  correct and still is, but the back up field used for non-paraxial particles had the wrong
  effective k1.
* Fix B field for the rf cavity field (`BDSFieldEMRFCavity` class). The direction of the vector was wrong
  due to a wrong translation from radial to Cartesian coordinates. Previously there was no variation in local
  `z`, which was wrong and has now been corrected.

**Geometry**

* Fix length of rbends being changed when specifying both `B` and `angle` and using a differet beam
  particle from the design particle. The input length would be ignored and the arc length recalculated
  based on the design beam particle.
* Fix caching of loaded geometry. A loaded piece of geometry will be reloaded (and possibly preprocessed)
  if loaded in another beam line component to ensure we generate a unique set of logical volumes. This
  fixes field maps, biasing, range cuts, regions and more being wrong if the same GDML file was reused
  in different components. However, this can be explicitly circumvented with the new parameter
  :code:`dontReloadGeometry` in a placement.
* Fix a bug where BDSIM would exit complaining about a conflicting material after loading a GDML
  file containing a material with the same name as one predefined in BDSIM.
* If a multipole has a zero-length, it will be converted in a thin multipole.
* Fixed issue where thin multipole & thinrmatrix elements would cause overlaps when located next to a dipole
  with pole face rotations. Issue #306.
* Fix missing magnet coil end pieces despite being available space when the sequence
  is a magnet, drift, element, or the reverse.
* Fix overlaps with various parameter combinations for an octagonal beam / aperture shape.
* Fixed issued where sections of an angled dipole were shorter than their containers, resulting in visual gaps
  in the geometry.
* Compilation fixes in AWAKE module for Geant4.11.1.0.
* Fix possible gap in angled geometry for `rectellipse` and `lhc` aperture types with strongly angled pole faces.
* Fix erroneous error about beam pipe being too big for a magnet when no magnet geometry was selected.

**Link**

* Fix nullptr materials for samplers in mass world. Have to explicitly use function to make it valid for developers.

**Output**

* Fix the wrong value being stored in PrimaryFirstHist.postStepProcessType which was in fact SubType again.
* When storing trajectories, it was possible if store transportation steps was
  purposively turned off that the first step point may not be stored. So, the pre-step
  was the creation of the particle and the post step was an interaction (i.e. not
  transportation). Previously, this step would not be stored breaking the indexing
  for parent step index.

**Physics Lists**

* :code:`em_extra`, :code:`muon`, and :code:`muon_inelastic` modular physics lists are now mutually exclusive.

**Parser**

* The input parser will now reject any duplicate object names (e.g. a field with the same name),
  whereas it didn't before. In the past, multiple objects would be created ignoring their name.
  However, after the input is loaded, BDSIM itself may look through the objects for one matching
  a name. In this case, the one it finds may be ambiguous or unexpected. The code was revised to
  purposively protect against this. This was always the case with beam line elements, but now it
  is also the case with all objects defined in the parser.
* Fix extension of all parser objects (i.e. not beam line elements), which was broken. Extension
  is the access and update of a variable inside a defined object such as a field or scorer.
* Fix parser :code:`print` command for all objects in the parser. Previously, only beam line elements
  would work with this command or variables in the input GMAD.
* The parser will reject any variable name that is the same as an option name. When editing
  option in input, a really common (hidden) error is that there's a semi-colon after an option.
  Therefore, the next option gets interpreted as a new constant or variable resulting in it
  having no effect at all. The parser will not prevent this from happening by complaining.
* Fixed ambiguous warning about variable redefinition.

**Sensitivity**

* Fix a bug where a sampler before a dump wouldn't record any output.
* Fix a bug where when turning off sensitive outers of magnets, an 'outer' loaded from
  an external geometry file such as GDML would remain sensitive.

**Tracking**

* When using the minimum kinetic energy option, tracks are now stopped in the stacking action
  rather than being allowed to be tracked for a single step. This should vastly improve the
  speed of some events with large numbers of tracks.
* Fix lack of user limits for RF cavity geometry.
* Fix maximum step length user limit for externally loaded geometry.
* Fix logic of building thin dipole fringe elements when using non-matrix integrator sets. As the
  rotated poleface geometry will be constructed in such circumstances, the thin integrated pole face kick
  is now not be applied as well. If finite fringe field quantities are specified, the thin elements will be built
  but will only apply the fringe kicks and not the pole face effects. If using a non-matrix integrator set
  and the option :code:`buildPoleFaceGeometry` is specified as false, thin pole face kicks will be applied.
* Fix calculation of the z position in the quadrupole integrator. Previously the step always advanced along z by the
  step length h regardless of the step's direction. Now, it advances along z by the projection of the step h onto
  the z axis. This change will only produce a noticeable impact on particles with a large transverse momentum,
  particularly those in low energy machines.
* Fix dipole integrator track when K1 is negative. The overall strength parameter calculated for the integrator matrices
  was incorrect when K1 < 0.

**Visualisation**

* GDML auto-colouring now works for G4 materials correctly. The name searching was broken. As a
  reminder, any material without a specific colour will default to a shade of grey according to
  its density. The auto-colouring is also fixed when preprocessing is used (the default).
* Fix visualisation of loaded GDML container volume.

**General**

* Fix double deletion bug for particle definition when using the Link version of BDSIM.
* Fix `distrFile` not being found when used as an executable option in the case where the
  current working directory, the main input gmad file and the distribution file were all in
  different places.
* Fix userfile distribution not finding ions by pdgid.
* "RINDEX" and "ABSLENGTH" optical parameters were fixed for the material properties
  definition of material "ups923a".
* "FASTCOMPONENT", "FASTTIMECONSTANT", and "YIELDRATIO" material properties for various optical
  materials have no effect when BDSIM is compiled with respect to Geant4 V11 onwards.
* Fix uncaught Geant4 exceptions by introducing our own exception handler to intercept
  the Geant4 one and throw our own, safely handled exceptions a la standard C++.
* Fix a bug where a particle could be misidentified as an ion and end up being a proton.
  An example would be "pion+" which doesn't match the correct "pi+" name in Geant4 but
  would pass through and become a proton despite its name.
* Fix runtime exception with Geant4 V11.1.0 for default options applied in BDSIM from all
  previous versions of Geant4 for epsilon max / min in all fields.

Output Changes
--------------
* Add angle of the element in the Model Tree.
* Add `samplerSPosition` in the Model Tree.
* Add `pvName` and `pvNameWPointer` to the Model Tree.
* Trajectories now have the variable `depth` for which level of the tree that trajectory is.
* Trajectories now have the variable `materialID`, which is an integer ID for each material
  for a given model. In the Model tree, a map of this integer to the name is stored. An integer
  is used to save space as it is stored for every step of each trajectory stored.
* Model tree now has two maps for material ID to name and vica-versa.
* Cavity info is now optionally stored in the Model Tree which includes rf element parameters and
  cavity geometry parameters. Default true.

Output Class Versions
---------------------

* Data Version 8.

+-----------------------------------+-------------+-----------------+-----------------+
| **Class**                         | **Changed** | **Old Version** | **New Version** |
+===================================+=============+=================+=================+
| BDSOutputROOTEventAperture        | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventBeam            | Y           | 5               | 6               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCavityInfo      | Y           | NA              | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimator      | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimatorInfo  | Y           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCoords          | N           | 3               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHeader          | Y           | 4               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHistograms      | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventInfo            | Y           | 6               | 7               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLoss            | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLossWorld       | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventModel           | Y           | 5               | 6               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventOptions         | Y           | 6               | 7               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventRunInfo         | N           | 3               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSampler         | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSamplerC        | Y           | NA              | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSamplerS        | Y           | NA              | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectory      | Y           | 4               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectoryPoint | Y           | 5               | 6               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTParticleData         | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+

Utilities
---------

These are no longer included directly with BDSIM but are available through pip. At the time
of writing, the corresponding versions of each utility are:

* pybdsim v3.3.2
* pymadx v2.0.1
* pymad8 v2.0.1
* pytransport v2.0.1


V1.6.0 - 2021 / 06 / 16
=======================

* Public CVMFS build now available. See :ref:`cvmfs-build`.
* HepJames is still the default random number generator, but you can now choose MixMax.

New Features
------------

* New executable option :code:`--version` for the bdsim executable that returns the version number.
* New skimming tool called :code:`bdskim` is included for skimming raw data. See :ref:`bdskim-tool`.
* New combination tool called :code:`bdsimCombine` is included to merge raw data files
  and skimmed data files alike. See :ref:`bdsim-combine-tool`.
* New ability to choose random number generator. Previously, BDSIM always used CLHEP's HepJamesRandom
  class. In more recent versions of Geant4, CLHEP's MixMax class is now the default. For now, BDSIM
  still uses HepJamesRandom as the default, but the user can select MixMax with the option :code:`randomEngine`.
* Few new variants of stainless steel at different temperatures as materials as well as RHC1000 plastic.
* :code:`fieldAll` can be specified for a geometry placement allowing a field to be attached to all volumes
  in that placement of geometry.
* Sub-fields can now be used with E field maps.
* BDSIM components can now be used in placements to place a single component anywhere in the world.
* The :code:`transform3d` beam line element now accepts axis angle parameters.
* Bias objects can now be attached to the world volume (e.g. the air) specifically.
* Bias objects can now be attached to the daughter volumes of the world when you load
  an external GDML world.
* By default now, the rest mass of an **artificially killed particle** is **not** included in the
  Eloss.energy hit recorded. If this is desired, as was the old behaviour in previous versions,
  then the option :code:`killedParticlesMassAddedToEloss=1` can be used.
* More granular control over information stored in trajectories. Trajectories can use a lot of disk
  space so it's important to allow this control so we store only what we need for every step of every
  trajectory chosen for storage.
* New beam / bunch distributions :code:`compositespacedirectionenergy` and :code:`box`. The first
  allows mixing of distributions for spatial, directional and energy / time rather than the usual
  coupled phase space of the `composite` distribution (e.g. x,xp and y,yp). `box` is uniform in
  all dimensions.
* A generic beam line :code:`element` type can now be marked as a collimator for the purpose of
  collimator histograms and summary information with the element definition :code:`markAsCollimator=1`.
* More colours for default material colours.
* New units accepted in input (PeV, PJ, GJ, MJ, kJ, J, mJ, uJ, nJ, pJ). J=1, GeV=1.
* New visualisation command :code:`/bds/samplers/view` to easily view samplers.
* New custom physics list interface to :code:`BDSIMClass` - see :ref:`interfacing-custom-physics`.
* "ModelTree" is now copied over when using `rebdsimCombine` to combine multiple `rebdsim`
  output files.
* New options:

.. tabularcolumns:: |p{0.30\textwidth}|p{0.70\textwidth}|

+----------------------------------+-------------------------------------------------------+
| **Option**                       | **Function**                                          |
+==================================+=======================================================+
| biasForWorldVacuum               | In the case of externally provided world geometry and |
|                                  | 'vacuum' volumes are named using the option           |
|                                  | `worldVacuumVolumeNames`, name(s) of bias object(s)   |
|                                  | can be given for these volumes.                       |
+----------------------------------+-------------------------------------------------------+
| biasForWorldVolume               | Name(s) of bias objects to be attached to the world   |
|                                  | logical volume only (i.e. not the daughters). White   |
|                                  | space separate list in a string.                      |
+----------------------------------+-------------------------------------------------------+
| biasForWorldContents             | Exclusively in the case of externally provided world  |
|                                  | geometry, the daughter volumes in the loaded world    |
|                                  | volume can be biased with this option. White space    |
|                                  | separated list in a string. Does not apply to world   |
|                                  | volume itself.                                        |
+----------------------------------+-------------------------------------------------------+
| dEThresholdForScattering         | The energy deposition in GeV treated as the threshold |
|                                  | for a step to be considered a scattering point.       |
|                                  | Along step processes such as multiple scattering may  |
|                                  | degrade the energy but not be the process that        |
|                                  | defined the step, so may not register. Default        |
|                                  | 1e-11 GeV.                                            |
+----------------------------------+-------------------------------------------------------+
| killedParticlesMassAddedToEloss  | Default 0 (off). When a particle is killed its rest   |
|                                  | mass will be included in the energy deposition hit.   |
|                                  | Relevant when minimumKineticEnergy option or          |
|                                  | stopSecondaries is used.                              |
+----------------------------------+-------------------------------------------------------+
| randomEngine                     | Name of which random engine ("hepjames", "mixmax").   |
|                                  | Default is "hepjames".                                |
+----------------------------------+-------------------------------------------------------+
| storeTrajectoryAllVariables      | Override and turn on `storeTrajectoryIon`,            |
|                                  | `storeTrajectoryLocal`,                               |
|                                  | `storeTrajectoryKineticEnergy`,                       |
|                                  | `storeTrajectoryMomentumVector`,                      |
|                                  | `storeTrajectoryProcesses`, `storeTrajectoryTime`,    |
|                                  | and `storeTrajectoryLinks`.                           |
+----------------------------------+-------------------------------------------------------+
| storeTrajectoryMomentumVector    | Store `PXPYPZ`, momentum (not unit) 3-vector in GeV   |
|                                  | for each step. Default False                          |
+----------------------------------+-------------------------------------------------------+
| storeTrajectoryKineticEnergy     | For the trajectories that are stored (according to    |
|                                  | the filters), store `kineticEnergy` for each step.    |
|                                  | Default True.                                         |
+----------------------------------+-------------------------------------------------------+
| storeTrajectoryProcesses         | Store `preProcessTyps`, `preProcessSubTypes`,         |
|                                  | `postProcessTypes`, `postProcessSubTypes`, the Geant4 |
|                                  | integer process IDs for pre and post step points.     |
|                                  | Default False.                                        |
+----------------------------------+-------------------------------------------------------+
| storeTrajectoryTime              | Store `T`, time in ns for each step. Default False.   |
+----------------------------------+-------------------------------------------------------+
| temporaryDirectory               | By default, BDSIM tries :code:`/tmp`, :code:`/temp`,  |
|                                  | and the current working directory in that order to    |
|                                  | create a new temporary directory in. Specify this     |
|                                  | option with a path (e.g. "./" for cwd) to override    |
|                                  | this behaviour.                                       |
+----------------------------------+-------------------------------------------------------+
| tunnelMaxSegmentLength           | Maximum permitted length of an automatic tunnel       |
|                                  | segment to be built (m). Default 50 m. Min 1 m.       |
+----------------------------------+-------------------------------------------------------+
| worldVacuumVolumeNames           | White space separated list of names as a string of    |
|                                  | logical volume names for volumes to be labelled as    |
|                                  | `vacuum` for the purpose of biasing.                  |
+----------------------------------+-------------------------------------------------------+

.. tabularcolumns:: |p{0.30\textwidth}|p{0.70\textwidth}|

+------------------------------------+--------------------------------------------------------------------+
| **Option**                         | **Description**                                                    |
+====================================+====================================================================+
| storeApertureImpactsHistograms     | Whether to generate the primary first aperture impact histogram    |
|                                    | `PFirstAI`, on by default.                                         |
+------------------------------------+--------------------------------------------------------------------+
| samplersSplitLevel                 | The ROOT splitlevel of the branch. Default 0 (unsplit). Set to 1   |
|                                    | or 2 to allow columnar access (e.g. with `uproot`).                |
+------------------------------------+--------------------------------------------------------------------+

General
-------

* The parser no longer builds a static library by default to save space and it responds to the
  option of :code:`BDSIM_BUILD_STATIC_LIBS` as the main libraries do. The parser library name
  has changed from "libgmadSharedLib" to "libgmad" and the static one is "libgmad-static".
* LHC dipole geometry now applies also to rbends as well as sbends.
* LHC dipole geometry now applies to hkickers and vkickers. In both cases the poles are like
  a normal LHC dipole (e.g. no "vertical" kicker geometry).
* In the case a rectellipse aperture is used but the parameters are such that the resultant
  shape would be an ellipse only, then elliptical solids are used to avoid overly complex
  Boolean solids and produce more efficient geometry. Such use of rectellipse as a default
  is common for the LHC. In the case where the result would be a circle, again, specific
  solids are used for optimisation of geometry. Applies to both straight and angled beam pipes.
  All done completely automatically internally.
* The print out of materials now lists the vacuum density in g/cm3 rather than g/m3, as is more common.
* The name of the bunch distribution is always print out in the terminal print out now.
* Clarified trajectory options in manual a bit - two tables, one for filtering, one for storage.
* Document option :code:`maximumTracksPerEvent`.
* The directory :code:`bdsim/examples/ILC` has been removed as this is an old unmaintained example
  that didn't work. This is in an effort to reduce the size of the examples and code repository generally.
* The default visualisation macro is now called "bdsim_default_vis.mac" so as not to be confused with
  the commonly named vis.mac, which makes it ambiguous as to which one is really being used.
* The visualisation macro path has the current working directory now as the last directory to search
  after the installation directory.
* Test program written for output Model tree functions.

Build Changes
-------------

* The event display executable "edbdsim" is not build by default with the CMake option
  :code:`USE_EVENT_DISPLAY` set to :code:`OFF` by default as this isn't maintained or finished.
* The CMake options have all been changed to start with :code:`USE_`.
* The ROOTSYS print out and option in BDSIM's CMake has been removed as this wasn't in fact
  used as a hint to CMake. The user should use :code:`-DROOT_DIR=/path/to/root` on the command
  line (standard CMake practice) if they want to specify a specific ROOT installation.
* Many Geant4 options for Qt and X11 have been marked as advanced to clean up the BDSIM ccmake
  list of options.
* The BDSIMConfig.cmake in the installation now contains all the compilation options but prefixed
  with :code:`BDS_`, for example, :code:`BDS_USE_HEPMC3`.
* If building a CMake project with respect to a BDSIM installation (i.e. using BDSIM), the variable
  :code:`BDSIM_INCLUDE_DIR` now correctly includes "bdsim" at the end.
* The bdsim.sh in the installation directory should now be portable and also work with zsh as well as bash.
* Test executable programs are no longer built by default and must be explicitly turned on
  with the CMake option :code:`BDSIM_BUILD_TEST_PROGRAMS`.

Bug Fixes
---------

* The options :code:`defaultBiasVacuum` and :code:`defaultBiasMaterial` didn't work - this has been fixed.
  The biasing wasn't attached to the volumes.
* Clarify message when loading a field map and header variables such as "nx" and "ny" were not
  specified and therefore defaulted to 0, which is invalid. Also, complain if these are purposively
  assigned to values less than 1.
* :code:`lhcdetailed` beam pipe now **ignores** :code:`beampipeMaterial` and uses the LHC specific
  materials as 2K.
* LHC magnet geometry provided with :code:`magnetGeometryType="lhcleft"` or "lhcright" has corrected
  materials now at 2K. If using this geometry style, the :code:`outerMaterial` global option as well
  as the per-element parameter will be ignored and the correct LHC materials used as per the LHC
  arc magnets.
* Fix LHC dipole fields which were on the wrong side for positive bend angles. This occurred due to
  a conflict with the logic of by default setting the yoke on the inner side of a bend for C-shaped
  dipole yokes.
* Fixed field in LHC magnet geometry second beam pipe. Now a duplicate of the vacuum field but with
  the opposite sign (for dipoles and quadrupoles).
* Warnings fixed if using LHC style geometry with a 0 angle bend from more recent versions of Geant4
  that complain about using a G4CutTubs when a G4Tubs is sufficient. The geometry was still valid, but
  is now marginally more efficient and the warnings are no longer present.
* Loaded GDML is now always visible. Geant4 would make the loaded GDML outermost volume invisible
  because GDML is designed for only one file as the world.
* BDSIM will correctly complain when no file is given for a field map. This is a common mistake
  when using both E and B fields. Previously, the code could segfault.
* Fix transforms for when an E or EM field was used in a component that was offset or tilted with
  respect to the beam line. The field would not correctly be aligned to the component. B fields were fine.
* User limits (minimum kinetic energy for example) weren't attached to placement geometry.
* Fix factor of 10 in field map strength for BDSIM-format field maps if the field components were not
  in the usual x, y, z order. i.e. X,Y,BY,BX,BZ would result in the field being a factor of 10 stronger.
* Fix S coordinate for energy deposition hit of a secondary particle that is killed. In the case where
  secondaries were killed, the S coordinate of that energy deposition hit would have been wrong.
* The curvilinear world and bridge world volumes and extra start and finish volumes are now
  consistent in their diameter.
* The `userfile` distribution would accept possibly conflicting information in coordinates such as
  E and Ek and P. It will now prevent this as it was ambiguous. In practice the order was just how
  it was read in the code, which was E, Ek, then P. Similarly for `S` and `z`.
* Fixed units on :code:`Event.Trajectory.energyDeposit`, which was in MeV and should be in GeV. Now in GeV.
* Fix possibly wrong overlap warning in a crystal collimator when using a cylinder or torus
  geometry. The overlap was calculated using the possibly large offset of the particular solid.
* `PrimaryFirstHit` and `PrimaryLastHit` are now filled for all primary particles when there are
  multiple removing the ambiguity of which one was recorded (no trackID etc was filled).
* If particles were killed in the world volume and :code:`storeElossWorld` was on, the kinetic energy
  of the tracks killed would not previously be added to the output. This has been fixed.
* Fix processing of a track in BDSSDEnergyDepositionGlobal that would have segfaulted if used.
* Fix recreation beam parameters which weren't loaded correctly. Provided the same input file was use, this
  wasn't a problem or noticeable. However, if a beam specific executable option such as
  :code:`--distrFile` was used, it would not be recreated properly. This has been fixed.
* Fix recreation when using trajectory storage options and AND logic.
* Fix possible scenario where range cuts weren't set in a recreation.
* Fix filtering of trajectories when using `storeTrajectoryTransportationSteps` and `trajectoryFilterLogicAND`
  together, which would result in no trajectories being stored.
* Fix uninitialised variable in BDSBunch.
* Fix energy being 1000x too big in the halo bunch distribution since the previous version. Units were multiplied
  through twice.
* Fix float / double casts in sampler output.
* Fix possible bad access by indexing beyond range of array in dipole fringe integrator.
* The maximum step length in a muon spoiler is now 1/20th of the length whereas before it was the full length.
  This step limit applies only in the 'yoke' (i.e. the outer part) of the spoiler and not in the pipe part.
* The trajectory function :code:`BDSOutputROOTEventTrajectory::primaryProcessPoint` only returned the process
  point the track was created by on the parent trajectory, not the primary. It is now fixed.
* The various trajectory functions now have been made tolerant of bad indices (e.g. negative numbers or parent
  used in a non-parent sense) and also of the now optional parts of the trajectory data.
* Fix Issue 297 where optics were incorrect due an uninitialised variable incorrectly setting dipole fringes
  to be zero strength.
* Fix possibly misidentified PrimaryFirstHit beam line elements (coordinates were always correct)
  that could in the case of some particles be either the very first step into the accelerator from
  air or the element before the expected one.
* Fix build with a modern compiler (e.g. GCC9) of ROOT and BDSIM. Specifically, if ROOT was compiled
  with C++14 or 17 the C++ standard for BDSIM is matched to that rather than the default C++11.
* Fixed the implementation of :code:`BDSOutputROOTEventModel::findNearestElement`.


Output Changes
--------------

* :code:`Event.Trajectory.energyDeposit` now in GeV - was previously actually MeV, so 1000x bigger value.
* Trajectory variables `PXPYPZ`, `T`, `preProcessTyps`, `preProcessSubTypes`, `postProcessTypes`,
  `postProcessSubTypes` are now **off** by default. These can be turned on in the output via new options
  listed above and in the options section. Expect a slight reduction in data file size when storing
  trajectories with default options.
* Trajectory variable `kineticEnergy` is now **on** by default.
* `PrimaryFirstHit` and `PrimaryLastHit` now have all primaries filled in, in the case there are multiple
  such as when using an event generator file.
* `trackID`, `partID`, `postProcessType`, `postProcessSubType` and `preStepKineticEnergy` are
  now all filled for the `PrimaryFirstHit` and `PrimaryLastHit` branches.
* New event summary variables `energyWorldExitKinetic` and `energyImpactingApertureKinetic`.
* A new vector of set variable names is stored in the options and beam trees in the output
  to ensure we recreate a simulation correctly.
* The trajectory filter bitset has been shortened by 1 to remove "transportation" as a filter.
  This was incorrectly used to filter the storage of complete trajectories.
* The class BDSOutputROOTEventTrajectoryPoint now has the member `stepIndex` to indicate the index
  of the step represented on the trajectory.


Output Class Versions
---------------------

* Data Version 7.

+-----------------------------------+-------------+-----------------+-----------------+
| **Class**                         | **Changed** | **Old Version** | **New Version** |
+===================================+=============+=================+=================+
| BDSOutputROOTEventAperture        | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventBeam            | Y           | 4               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimator      | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimatorInfo  | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCoords          | N           | 3               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHeader          | N           | 4               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHistograms      | N           | 3               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventInfo            | Y           | 5               | 6               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLoss            | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLossWorld       | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventModel           | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventOptions         | Y           | 5               | 6               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventRunInfo         | N           | 3               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSampler         | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectory      | N           | 4               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectoryPoint | Y           | 4               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTParticleData         | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+

Utilities
---------

* pybdsim v2.4.0
* pymadx v1.8.2
* pymad8 v1.6.1
* pytransport v1.5.0


V1.5.1 - 2020 / 12 / 21
=======================

Hotfix for tapered elliptical collimators (`ecol`). The apertures would differ at the few percent
level due to the calculation of the obscure parameterisation of the solid used in Geant4.

V1.5.0 - 2020 / 12 / 16
=======================

Build System
------------

* CMake 3.1 now required.
* CMake CXX Standard now used (C++11) instead of individual compiler flags.

New Features
------------

* Scoring meshes and scorers have been introduced that allow 3D scoring meshes to be used and
  created per event 3D histograms for various quantities. Ability to score multiple quantities,
  per particle, with material exclusion are included. See :ref:`scoring` for details on usage.
* More granular control over output and a minimal output option.
* Both the design and beam particle may now be specified by either :code:`energy` (total),
  :code:`kineticEnergy`, :code:`momentum` in the case of the design particle, or :code:`E0`,
  :code:`Ek0` and :code:`P0` in the case of the optional beam particle if different from
  the design one. This makes input of the desired beam easier but also permits easy specification
  of different particle species beams with the same momentum for example.
* Either :code:`sigmaE`, :code:`sigmaEk` or :code:`sigmaP` can be used for bunch distributions
  that use :code:`sigmaE` nominally.
* Normalised emittance can now be used for `gausstwiss` and `halo` distributions with the parameters
  :code:`emitnx` and :code:`emitny`.
* Loaded geometry can now be auto-coloured by BDSIM whereas before it would all appear very light grey.
  The colouring has a few specific ones, but is mostly grey by density, and the opacity is also varied
  depending on the state of the material. The parameter :code:`autoColour` can be used with the
  generic beam line element as well as placements and magnet outer geometry and is on by default.
* BLMs now must use a :code:`scoreQuantity` to name a scorer object to decide what they record
  as opposed to previously just recording energy deposition.
* BLMs now have a parameter :code:`bias` that allows a cross-section biasing object to be attached
  to all logical volumes in that BLM.
* Cubic is now the default interpolation for fields and is automatically matched to the number
  of dimensions in the field map file.
* The interpolation for fields need only be specified by name and will be matched to the number
  of dimensions generally. e.g. 'cubic' instead of 'cubic2d'. The older names are still accepted but
  if BDSIM will give an error if the wrong dimension is used.
* LHC yoke fields that are the sum of two multipole yoke fields. Works for rbend, sbend, quadrupole
  and sextupole. Default on and controlled by the new option :code:`yokeFieldsMatchLHCGeometry`.
* Ability to filter out unstable particles with no default decay table in Geant4 when loading event
  generator files for a beam - now the default behaviour and controllable with the beam parameter
  :code:`removeUnstableWithoutDecay`.
* Interpolator types for fields don't need the dimension as a suffix any longer. e.g. 'cubic' is now
  sufficient instead of one of 'cubic1d', 'cubic2d', 'cubic3d', 'cubic4d'. It is now automatically
  determined from the dimensions of the field type. Old syntax is still accepted.
* A field may now have a :code:`magneticSubField` where a smaller field is overlaid on top of a main
  field providing the ability to place a smaller more detailed magnetic field map on top of a larger
  (perhaps coarser) one. The boundaries exist separately.
* Variable bin widths may be used in rebdsim analysis configurations as supplied by an external text file
  in any dimension.
* A histogram for the primary first impact in `S` is now generated by default, similar to `ELoss`.  This
  is controlled with the new option `storeApertureImpactsHistograms`.
* Ability to store only detailed production point of certain kinds of particles through new options
  for the trajectory storage.
* New ability to run a Geant4 macro before a run. Useful to adjust physics list parameters in Geant4
  reference physics lists such as extra muon processes in the EM extra physics list.
* New options:

.. tabularcolumns:: |p{0.30\textwidth}|p{0.70\textwidth}|

+------------------------------------+--------------------------------------------------------------------+
| **Option**                         | **Description**                                                    |
+====================================+====================================================================+
| autoColourWorldGeometryFile        | Boolean whether to automatically colour geometry loaded from the   |
|                                    | worldGeometryFile. Default true.                                   |
+------------------------------------+--------------------------------------------------------------------+
| beamPipeIsInfiniteAbsorber         | When turned on, all particles that hit the material of the beam    |
|                                    | pipe are killed and the energy recorded as being deposited there.  |
+------------------------------------+--------------------------------------------------------------------+
| geant4PhysicsMacroFileName         | The name of a text macro file with commands that are suitable for  |
|                                    | the Geant4 interpreter that will be executed after the physics     |
|                                    | list is constructed but before a run.                              |
+------------------------------------+--------------------------------------------------------------------+
| outputCompressionLevel             | Number that is 0-9. Compression level that is passed to ROOT's     |
|                                    | TFile. Higher equals more compression but slower writing. 0 is no  |
|                                    | compression and 1 minimal. 5 is the default.                       |
+------------------------------------+--------------------------------------------------------------------+
| physicsVerbosity                   | Set the physics verbosity for Geant4 (0,1,2).                      |
+------------------------------------+--------------------------------------------------------------------+
| storeApertureImpactsHistograms     | Whether to generate the primary first aperture impact histogram    |
|                                    | `PFirstAI`, on by default.                                         |
+------------------------------------+--------------------------------------------------------------------+
| storeElossPhysicsProcesses         | Store the post step process ID and sub-ID for the step.            |
+------------------------------------+--------------------------------------------------------------------+
| storeMinimalData                   | When used, all optional parts of the data are turned off. Any bits |
|                                    | specifically turned on with other options will be respected.       |
+------------------------------------+--------------------------------------------------------------------+
| storeParticleData                  | Control whether the basic particle data is stored in the output    |
|                                    | for all particles used or not. Renamed from `storeGeant4Data`.     |
+------------------------------------+--------------------------------------------------------------------+
| storePrimaries                     | Boolean, true by default. If false, don't fill the Primary branch  |
|                                    | of the Event tree in the output. Useful to minimise file size.     |
+------------------------------------+--------------------------------------------------------------------+
| storePrimaryHistograms             | Whether to generate summary histograms of the primary first hit    |
|                                    | and loss point versus S coordinate per event. On by default.       |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryStepPoints          | Integer number of step points to store for each trajectory that is |
|                                    | chosen to be stored. Should be greater than 1. Storing 1 will mean |
|                                    | only the first creation point is stored.                           |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryStepPointLast       | Boolean. If true, and used in combination with the option          |
|                                    | `storeTrajectoryStepPoints`, the end point of the trajectory is    |
|                                    | also stored.                                                       |
+------------------------------------+--------------------------------------------------------------------+
| yokeFieldsMatchLHCGeometry         | Boolean whether to use yoke fields that are the sum of two         |
|                                    | multipole yoke fields with the LHC separation of 194 mm. Default   |
|                                    | true. Applies to rbend, sbend, quadrupole and sextupole.           |
+------------------------------------+--------------------------------------------------------------------+
| storeApertureImpactsHistograms     | Whether to generate the primary first aperture impact histogram    |
|                                    | `PFirstAI`, on by default.                                         |
+------------------------------------+--------------------------------------------------------------------+
| samplersSplitLevel                 | The ROOT splitlevel of the branch. Default 0 (unsplit). Set to 1   |
|                                    | or 2 to allow columnar access (e.g. with `uproot`).                |
+------------------------------------+--------------------------------------------------------------------+


General
-------

* The maximum step length in a field map is by default now the minimum spatial distance in the field
  map loaded. Previously, it was the full length of the element the field map was attached to. The user
  can still set the :code:`maximumStepLength` parameter in the field definition to reduce this further,
  but the minimum of the numbers supplied will be used.
* Previously, the absolute minimum "maximum step length" in a field map was 1mm. This has been reduced
  to 1 micron. This is irrespective of what the user specifies in the field description.
* Shared library now the default for BDSIM. The CMake option :code:`BDSIM_BUILD_STATIC_LIBS`
  allows the static library to be compiled too (in addition to the shared one).
* rebdsimCombine will exit if the first argument, which is meant to be the single output file, contains
  an asterisk (*), as it is likely the arguments are in the wrong order.
* BDSIM will exit if the option :code:`minimumKineticEnergy` is set to a value higher than the kinetic
  energy of the beam particle. This takes into account the possibly different kinetic energy of the beam
  particle versus the design particle. Such a combination of options would result in all primary particles
  being immediately killed and not tracked through the model.
* Linear and cubic interpolation implementation has be switched from hard coded types to templates. No
  difference in results, but this makes the code usable elsewhere.
* :code:`composite` distribution now defaults to :code:`reference` distribution for each dimension, so
  if a given dimension isn't specified it'll be the default.
* Warn the user if a sampler placement is renamed due to a beam line sampler already having that name.
* PDG Particle ID token when defining the columns in a :code:`userfile` distribution has been changed from "pt" to
  "pdgid". "pt" is no longer a valid token.
* When loading multiple GDML files, a material specified may have the same name in different files butt
  be different. This could result in the incorrect material being used in the second file. We have this
  issue with geometry (solids and logical volumes) also, hence our GDML pre-processing. We now cache the
  names of the materials and warn if a duplicate is defined. Normally, this would pass through without
  error and result in possibly the other material being found by Geant4.

Bug Fixes
---------

* Tolerate "-" character in rebdsim input and output file names written inside the analysis configuration
  file. In the past, only the part up to this character was used.
* Fix loading of a visualisation macro (:code:`--vis_mac=vis.mac`) when the file was called vis.mac and was
  in the current directory. Previously, the bdsim default one would always be used. Fixed by appending the
  current working directory to the macro search path for Geant4.
* Fixed inspection of G4CutTubs extent in BDSGeometryInspector that was used when a GDML file was loaded
  with a G4CutTubs as the container (outermost) solid.
* Fixed bug in dipole fringe scaling with the `bdsimmatrixfringescaling` integrator set where the particle
  bending radius was scaled twice and arguments were passed into the base stepper in the wrong order.
* Dipole fringes now apply the fringe kick and dipole transport in the correct order depending on if the fringe
  is at the entrance or exit of the dipole.
* Fix wrong variable name print out for halo beam distribution.
* Improve cryptic error for wrongly specified composite beam distribution.
* Units for :code:`kineticEnergy` and :code:`rigidity` variables in the trajectory output were fixed.
* Throw an error if a sampler placement has a conflicting name with something already in the output rather
  than potentially just overwrite it or it not appear properly.
* Fix the field from E and EM fields when they were offset with respect to a beam line element.
* Fixed bug when not storing primaries using the option `storePrimaries` where the primary branch was added to
  the sampler list and therefore appeared empty in the output. This consequently caused rebdsimOptics to segfault.
* Fix tapering in an :code:`ecol`, which wouldn't be tapered in v1.4.
* Fix calculation of minimum geometry radius in magnets for G4CutTubs used for intersection
  (to give the angled faces), which was dominated by the horizontal size. In cases with magnets
  that were taller than they were wide and with extremely strong bending angles or pole faces
  this could have produced geometry Geant4 would complain about. Fixed in
  :code:`BDSMagnetOuter::MinimumIntersectionRadius()`.

Output Changes
--------------

These are very important changes to take note of:

* A new option :code:`storeMinimalData` has been introduced that turns off all optional parts of the data
  reducing the file size. Options that explicitly turn on parts of the data will be respected.
* If optional information is **not** stored, the branches in the Event Tree will **not** be written to
  keep the data structure as simple as possible. The analysis and DataLoader classes are tolerant of this.
* The PrimaryGlobal variables are now all capital (e.g. :code:`X` instead of :code:`x`) to be consistent
  that they are global coordinates and not local coordinates.
* The class :code:`BDSOutputROOTGeant4Data` has been renamed to :code:`BDSOutputROOTParticleData` to be
  clearer. The analysis `DataLoader` class will not be able to load this branch in older data. In this case,
  the version of BDSIM used to create the data should be used, or the data inspected directly as required -
  the previous data will always be readable by ROOT. The members of the class and the functionality are
  exactly the same, but the ROOT dictionary generation feature to allow renaming of a class doesn't seem
  to work in practicality.
* Samplers now have a variable `p` which is the momentum of the particle in GeV.
* Model tree now has scoring mesh global placement transforms and names stored to aid visualisation later on.
* The various storage Boolean options for the BDSOutputROOTEventLoss class have been removed from
  the output as these are only needed at run time and are not needed as a copy for each event in the output.
  The options preserve what was stored and it is not expected that these change between events so this
  was completely unnecessary and wasteful.
* Units for :code:`kineticEnergy`, :code:`rigidity` and :code:`mass` variables in the trajectory output were
  fixed. These were previously in Geant4 units but are now consistent with those in the rest of the output
  such as samplers and are GeV and Tm.
* In :code:`BDSOutputROOTEventTrajectory` and :code:`BDSOutputROOTEventTrajectoryPoint` classes, the track ID
  variables have been changed to be unsigned int type.
* New variables in header for whether the BDSIM output file is a skimmed file or not - future proofing for
  upcoming skimming program.

Output Class Versions
---------------------

* Data Version 6.

+-----------------------------------+-------------+-----------------+-----------------+
| **Class**                         | **Changed** | **Old Version** | **New Version** |
+===================================+=============+=================+=================+
| BDSOutputROOTEventAperture        | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventBeam            | N           | 4               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCoords          | Y           | 2               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimator      | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimatorInfo  | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHeader          | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHistograms      | N           | 3               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventInfo            | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLoss            | Y           | 4               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLossWorld       | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventModel           | Y           | 4               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventOptions         | N           | 5               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventRunInfo         | N           | 3               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSampler         | Y           | 4               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectory      | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectoryPoint | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTGeant4Data (\*)      | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTParticleData         | Y           | NA              | 1               |
+-----------------------------------+-------------+-----------------+-----------------+

* (\*) deprecated in favour of the renamed class BDSOutputROOTParticleData

Utilities
---------

* pybdsim v2.3.0
* pymadx v1.8.1
* pymad8 v1.6.0
* pytransport v1.4.0

V1.4.0 - 2020 / 06 / 08
=======================

Expected Changes To Results
---------------------------

* Any wirescanner elements should be updated to use :code:`wireAngle` instead of :code:`angle` for
  their rotation angle. Not doing this will result in different angles and therefore results.
* Fix for field maps with rotations (multiples of :math:`\pi/2` were ok). The field will now be correct
  but this may be different from previous releases.
* Field maps now pick up the tilt from the element, so a separate tilt isn't required in the field
  definition as was in the past to make the field align with a tilted element. In this case, the field
  definition tilt should be removed and the field will be orientated to the component it's attached to.
* PrimaryFirstHit location on wire scanners will now be more accurate, where it might have missed it before.
* Default range cut from BDSIM will not be enforced if using a Geant4 physics list. It will only be set if
  specified in the user input.
* Neutrinos are no longer killed by default. They can be turned off (for optimisation purposes) with
  the option :code:`option, killNeutrinos=1;`.
* The default when using the :code:`option, storeTrajectories=1;` is to only store the primary trajectory,
  which will vastly reduce the data size. See output changes below for further details.
* Trajectory option :code:`storeTrajectoryELossSRange` is now in metres and not millimetres.
* Reference coordinates `X0`, `Y0`, `Z0`, `Xp`, `Yp` are now added to the userfile distribution
  coordinates if specified. (`Zp` was already added).
* Polarity of dipole yoke fields was fixed so particles slightly outside the beam pipe will be deflected
  in a different (but now correct) direction.
* Merged **simple** histograms (only simple ones) from using rebdsimCombine are now truly the sum, whereas
  in the past they were the mean.
* Note a change of sign to the left crystal angle. A positive angle and also bendingAngleAxisY rotates
  both left and right crystals away from the centre of the collimator. Will only affect the left crystal
  as compared to previous behaviour.

New Features
------------

* BDSIM no longer requires a beam line to be built! You can simply make a placement or even an empty world.
* Restructured "Model Description" section in the manual as it was growing overly big and difficult to use.
* New units: `twopi`, `halfpi` and `PeV`.
* New bunch distribution `sphere` to generate random directions at a given point.
* `S0` for bunch offset in curvilinear frame now a documented feature of the bunch.
* Improved event level verbosity.
* All verbosity options now documented, including corresponding executable options.
* BDSIM will now exit if invalid ranges and bins are specified for the single 3D
  energy deposition ('scoring') histogram that can be specified via options.
* New verbose event stepping options. See :ref:`bdsim-options-verbosity` for more details.
* New beam loss monitors (BLMs) with :code:`blm` command (See :ref:`detectors-blms`).
* New executable option :code:`--distrFileNLinesSkip` for the number of lines to skip into
  a distribution file.
* New executable option :code:`--nturns` to control the number of turns in a circular machine.
* Support for partially stripped ions in output samplers.
* Optional linking to HepMC3 for event generator output file loading. Can load any format
  HepMC3 can load.
* Filters for event generator particles loaded with HepMC3.
* Ability to print out all particles and physics processes to be helpful for finding Geant4
  names for biasing. See new options below.
* `kaon-`, `kaon+` or `kaon0L` may now be used as beam particles.
* The beam particle may now be specified by its PDG integer ID rather than by name.
* A new physics list called "all_particles" has been introduced to construct all particles
  only but no physics processes. Useful for an exotic beams where only tracking is required.
* New `tilt` parameter for the beam command to apply a rotation about unit Z after the coordinates
  are generated as an easy method to introduce coupling.  Note, this is in the beam command.
* The userfile bunch distribution now supports the column "S" to allow specification of curvilinear
  coordinates as input.
* Field maps are now automatically tilted when attached to a tilted beam line element, whereas
  they weren't before.
* RF cavity fringe fields have been implemented and are on by default. They are controlled with
  the `includeFringeFieldsCavities` option. The `includeFringeFields` option does not affect cavity fringes.
* Revised executable options for verbosity. These are now the exact same as the input options. Old
  options are still functional but undocumented.
* Added the ability to attach a BLM flush to the side of a component
  with option `side`, including the possibility of introducing an additional gap with `sideOffset`.
* New internal region class allows better setting of defaults when defining custom regions. Previously,
  these would just be the default in the class if they weren't specified, which was 0. The global ones
  will now take precedence as will the value `defaultRangeCut` in the `cutsregion` declaration.
* Added the ability to attach a BLM flush to the side of a component
  with option `side`, including the possibility of introducing an additional gap with `sideOffset`.
* New options `apertureImpactsMinimumKE` and `collimatorHitsMinimumKE` to control the minimum kinetic
  energy a particle must have for either an aperture impact or collimator hit respectively to
  be generated.
* A generic element now has the ability to label (classify) volumes as 'vacuum' for the purposes of
  biasing where we split geometry into 'vacuum' and (general) 'material', e.g. yoke. See :ref:`element`
  for details and the :code:`namedVacuumVolumes` parameter.

* New options:

.. tabularcolumns:: |p{0.30\textwidth}|p{0.70\textwidth}|

+------------------------------------+--------------------------------------------------------------------+
| **Option**                         | **Description**                                                    |
+====================================+====================================================================+
| apertureImpactsMinimumKE           | Minimum kinetic energy for an aperture impact to be generated (GeV)|
+------------------------------------+--------------------------------------------------------------------+
| collimatorHitsminimumKE            | Minimum kinetic energy for a collimator hit to be generated (GeV)  |
+------------------------------------+--------------------------------------------------------------------+
| includeFringeFieldsCavities        | Include thin fringe fields for RF cavities only, on by default.    |
|                                    | Cavity fringes are not affected by the includeFringeFields option, |
|                                    | includeFringeFieldsCavities must be explicitly turned off if no    |
|                                    | fringes are to be built at all in the model.                       |
+------------------------------------+--------------------------------------------------------------------+
| preprocessGDMLSchema               | Whether to preprocess a copy of the GDML file where the URL of     |
|                                    | the GDML schema is changed to a local copy provided in BDSIM so    |
|                                    | geometry can be loaded without internet access. On by default.     |
+------------------------------------+--------------------------------------------------------------------+
| printPhysicsProcesses              | Print out all defined particles according to the physics list and  |
|                                    | the names of all defined physics processes for that particle.      |
+------------------------------------+--------------------------------------------------------------------+
| storeApertureImpacts               | Create an optional branch called "ApertureImpacts" in the Event    |
|                                    | tree in the output that contains coordinates of where the primary  |
|                                    | particle exists the beam pipe. Note this could be multiple times.  |
+------------------------------------+--------------------------------------------------------------------+
| storeApertureImpactsIons           | If `storeApertureImpacts` is on, the information will be generated |
|                                    | for all secondary ions as well as the primary. No information will |
|                                    | be generated for other particles.                                  |
+------------------------------------+--------------------------------------------------------------------+
| storeApertureImpactsAll            | If `storeApertureImpacts` is on, the information will be generated |
|                                    | for all particles leaving the beam pipe when this option is turned |
|                                    | on.                                                                |
+------------------------------------+--------------------------------------------------------------------+
| storeCollimatorHits                | Store collimator hits for primary particles. This is addition to   |
|                                    | the basic `primaryInteracted` and `primaryStopped` variables.      |
+------------------------------------+--------------------------------------------------------------------+
| storeCollimatorHtisLinks           | `storeCollimatorLinks` has been renamed to this (backwards         |
|                                    | compatible.                                                        |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryIon                 | For the trajectories that are stored (according to the filters),   |
|                                    | store `isIon`, `ionA`, `ionZ` and `nElectrons` variables.          |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryLocal               | For the trajectories that are stored (according to the filters),   |
|                                    | store `xyz` and `pxpypz` local coordinate variables.               |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryLinks               | For the trajectories that are stored (according to the filters),   |
|                                    | store `charge`, `kineticEnergy`, `turnsTaken`, `mass` and          |
|                                    | `rigidity` variables for each step.                                |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryTransportationSteps | On by default. Renamed and opposite logic to                       |
|                                    | `trajNoTransportation` option.                                     |
+------------------------------------+--------------------------------------------------------------------+
| trajectoryFilterLogicAND           | False by default. If set to true (=1) only particles that match    |
|                                    | of the specified filters will be stored. This is opposite to the   |
|                                    | more inclusive OR logic used where a trajectory will be stored if  |
|                                    | matches any of the specified filters.                              |
+------------------------------------+--------------------------------------------------------------------+
| verboseRunLevel                    | (0-5) level of Geant4 run level print out. The same as             |
|                                    | `-\\-verboseRun=X` executable option.                              |
+------------------------------------+--------------------------------------------------------------------+
| verboseEventBDSIM                  | Extra print out identifying the start and end of event             |
|                                    | action as well as the allocator pool sizes. Print out              |
|                                    | the size of each hits collection if it exists at all. The          |
|                                    | same as `-\\-verboseEventBDSIM` executable option.                 |
+------------------------------------+--------------------------------------------------------------------+
| verboseEventStart                  | Event index to start print out according to                        |
|                                    | `verboseEventBDSIM`. Zero counting.                                |
+------------------------------------+--------------------------------------------------------------------+
| verboseEventContinueFor            | Number of events to continue print out event information           |
|                                    | according to `verboseEventBDSIM`. -1 means all subsequent          |
|                                    | events.                                                            |
+------------------------------------+--------------------------------------------------------------------+
| verboseEventLevel                  | (0-5) level of Geant4 event level print out for all events.        |
+------------------------------------+--------------------------------------------------------------------+
| verboseSteppingBDSIM               | Extra print out for all steps of all particles from BDSIM          |
|                                    | for events in the range according to `verboseSteppingEventStart`   |
|                                    | and `verboseSteppingEventContinueFor`. Default is all events.      |
+------------------------------------+--------------------------------------------------------------------+
| verboseSteppingLevel               | (0-5) level of Geant4 print out per step of each particle. This    |
|                                    | done according to the range of `verboseSteppingEventStart`, and    |
|                                    | `verboseSteppingEventContinueFor`. Default is all events and all   |
|                                    | particles.                                                         |
+------------------------------------+--------------------------------------------------------------------+
| verboseSteppingEventStart          | Event offset (zero counting) to start stepping print out           |
|                                    | according to `verboseSteppingLevel`.                               |
+------------------------------------+--------------------------------------------------------------------+
| verboseSteppingEventContinueFor    | Number of events to continue print out stepping information for    |
|                                    | according to `verboseSteppingLevel`.                               |
+------------------------------------+--------------------------------------------------------------------+
| verboseSteppingPrimaryOnly         | If true, only print out stepping information for the primary.      |
+------------------------------------+--------------------------------------------------------------------+
| verboseImportanceSampling          | (0-5) level of importance sampling related print out.              |
+------------------------------------+--------------------------------------------------------------------+
| verboseStep                        | Whether to use the verbose stepping action for every               |
|                                    | step. Note, this is a lot of output.                               |
+------------------------------------+--------------------------------------------------------------------+
| verboseSteppingLevel               | (0-5) level of Geant4 stepping level print out. The same           |
|                                    | as `-\\-verbose_G4stepping=X` executable option.                   |
+------------------------------------+--------------------------------------------------------------------+
| verboseTrackingLevel               | (0-5) level of Geant4 tracking level print out. The same           |
|                                    | as `-\\-verbose_G4tracking=X` executable option.                   |
+------------------------------------+--------------------------------------------------------------------+

* Previous verbosity options are still valid but now undocumented. This change is to make the naming consistent
  in lowerCamelCase and to make executable options consistent with input gmad options.


General
-------

* Installation support for AFS has been dropped since this is reaching end of life at CERN and may builds
  there are unmaintained or now on cvmfs.
* Executable verbosity options, now accepted in input gmad.
* Valid default ranges for general single 3D energy deposition 'scoring' histogram
  available through options. Now 1m in x,y,z with 1 bin.
* wirescanner element now uses :code:`wireAngle` for the rotation angle and not :code:`angle`.
* wirescanner element now requires a material to be specified as this makes a large difference
  to the expected result. This should be specified.
* Sampler hits now store rigidity, mass and charge as these are only correct from the G4DynamicParticle
  and cannot be reliably or easily back-calculated afterwards based on the particle definition (PDG ID)
  for partially stripped ions. This storage marginally increases the memory usage per sampler hit, so
  a small increase in memory (RAM) usage may be observed for very large numbers of sampler hits.
* Crystals in crystal collimators are now sensitive as collimators and produce the special collimator
  hit information in the output. The crystal channelling process is ignored as a step defining process
  for generating unique hits in the crystal.
* All processes of type `G4ProcessType::fNotDefined` are excluded from generating collimator specific hits.
* The option `storeCollimatorInfo` now does not store collimator hits for primary particles but only
  the Boolean variables `primaryInteracted` and `primaryStopped` as well as `totalEnergyDeposited` in
  each per-collimator branch in Event. This allows greater control over the amount of information stored.
  The primary hits can be turned on as well with the option `storeCollimatorHits`.
* Remove use of exit(1) throughout the code.
* Element variables "blmLocZ" and "blmLocTheta" were old and removed. These will be rejected in any
  element definition from now on.
* The generic beam line "element" will now be inspected for end piece coil placement on the edge of magnets
  and these will be placed if the pro or preceding geometry is small enough. Previously, coils would only be
  placed if (strictly) drifts were on either side of the magnet.
* When using a Geant4 reference physics list the default is to use BDSIM's ranges. This can be turned off,
  but shouldn't interfere if no ranges are set. This has been changed as the `defaultRangeCut` would be enforced
  in the past even if not set explicitly by the user, causing BDSIM's default 1 mm range to be used.
* `option, checkOverlaps=1;` now checks the internal structure of any loaded GDML geometry. Previously,
  only the placement of the container volume of the loaded geometry was checked to see if it overlaps
  with any other geometry, but nothing internally.
* Neutrinos are no longer killed by default. They can be turned off (for optimisation purposes) with
  the option :code:`option, killNeutrinos=1;`.
* Rectellipse beam pipe will now use elliptical beam pipe without the use of Boolean solids in cases
  where the parameters result in this. This makes therefore a marginally simpler model and avoids
  abusing unnecessary Booleans in Geant4 due to the way people use the rectellipse for everything.
* Revised calculation of octagonal beam pipe points such that each side is uniformly thick exactly
  equalling beam pipe thickness. This is an improvement over the previous algorithm for this.
* Descriptions of the elements rmatrix and thinrmatrix have been added to the manual.
* Maximum step size calculation for RF cavities has been improved to use 2.5% of the minimum of
  the wavelength (based on the frequency of the cavity and only valid when non-zero frequency)
  and the length of the element.
* Degrader wedges are no longer connected with geometry to prevent overlaps. Degrader can now be fully open
  when using the element parameter :code:`degraderOffset`.

Bug Fixes
---------

* Fix polarity for dipole yoke fields. The field in the yokes had the opposite polarity to that
  of the beam pipe resulting in particles slightly missing the beam pipe being deflected in the
  wrong direction.
* Fix phase offset based on postiion in lattice for RF cavities. Only noticeable when the phase
  was set to provie zero acceleration (:math:`pi/2`) and it was slightly off causing a gain or
  loss in energy.
* Fixed formula in manual for standard error on the mean calculation. The implementation in code
  was correct and has not changed.
* Fix thick multipole element where the field was 1M times too strong because of the omission of units.
* Fix Issue #272 where there could be a possible segfault due to the beam particle definition being
  updated when multiple different particles were used for a `userfile` distribution.
* Errors in 2D and 3D merged histograms from events were 0 always. The mean was corrected, but the error
  was not filled correctly - this has been fixed.
* Merged **simple** histograms (only simple ones) from using rebdsimCombine are now truly the sum, whereas
  in the past they were the mean.
* Fix for potential segfault when analysing collimator information branches in event tree. Dependent
  on number of collimators analysed causing std::vector to reallocate and invalidate address of
  pointers as required by ROOT.
* Fix for warnings about unknown collimator branch names when loading data with DataLoader class.
* Fixed warnings about exiting when Geant4 geometry in closed state in the event
  of a warning being produced and BDSIM exiting. Now correctly intercept and re-throw
  the exception.
* Fix a bug where setting a rotation angle for a wire scanner would result in energy deposition
  S coordinates all being -1. This was because the :code:`angle` parameter is assumed to only
  ever be for bends and BDSIM reduces the sampler and curvilinear world (used for coordinate
  transforms) diameter given the maximum bending angle of bends in the whole lattice. This is
  required to avoid overlaps before construction. The new parameter :code:`wireAngle` is used
  instead.
* Fix wire scanner sensitivity. The wire was never sensitive.
* Fix generic element sensitivity. It never produced energy deposition.
* Partial fix for aggressive looping particle killing in Geant4.10.5. For electrons and positrons,
  and the beam particle, the looping threshold has be lowered to 1 keV. Ongoing investigation.
* Fix missing previous single 3D scoring map (3D histogram of machine energy deposition)
  being missing from the run histograms.
* The rigidity was corrected for partially stripped ions in the sampler output.
* The initial kinetic energy of partially stripped ions was slightly inflated due to subtracting
  the nuclear mass not including the mass of the electrons. The magnetic fields were however
  calculated correctly and this resulted in incorrect behaviour. This has been since fixed.
* Fix a bug where if a userfile with different particle types was used and `-\\-generatePrimariesOnly`
  was used the phase space coordinates would be correct but the mass, charge, rigidity would be
  written wrongly to the output. The particle definition is now updated correctly in the special
  case of generating primaries only where the Geant4 kernel isn't used.
* Fix a possible segfault when an ion beam is used for as well as the `-\\-generatePrimariesOnly`
  excutable option.
* Ion variables are now correctly written to the Primary branch of the Event tree in the case of using
  an ion beam with `-\\-generatePrimariesOnly`.
* Fix crystal channelling biasing that was broken with commit #66a6809. This was introduced between
  v1.3.1 and v1.3.2. It resulted in the channelling working but the cross-section biasing not being
  applied and therefore the rest of the physics processes acting as if the block was amorphous.
* Fix crystal positioning in `crystalcol`. Previously, the crystal centre was placed at `xsize` but
  it should be in the inside edge to match other collimators. The inside of the edge is now aligned
  to `xsize`.
* Note a change of sign to the left crystal angle. A positive angle and also bendingAngleAxisY rotates
  both left and right crystals away from the centre of the collimator. Will only affect the left crystal
  as compared to previous behaviour.
* Fix `e1`, `e2`, `hgap`, `fint`, `fintx`, `fintk2`, `fintxk2` not being filled in Model tree output.
  They're now filled correctly.
* Fix generic biasing for protons when an ion is used as the beam, or when GenericIon is available in
  the physics list and also biased. Previously, the proton would not be biased but instead only the
  ions would be.
* Fix Event.Summary.memoryUsageMb which was always 0. Also now correct units on linux and Mac. Was previously
  a factor of 1048 too big on linux.
* Fix scaling of relativistic beta in the dipolequadrupole integrator, the particle design beta was
  always was used before regardless of dipole scaling.
* Fix phase term in rf field when frequency is 0. When frequency is 0, the field should be constant and
  maximal, however, it was constant but still modulated by the phase of the incoming particle.
* Fix for default value of "energy" (actually energy loss) in the trajectory branch of the Event tree
  where the default value was -1 whereas it should be 0.
* Fix missing geometrical margins in undulator.
* Fix small occasional overlap with rectellipse beam pipe with yoke of magnets.
* Fix a lack of warning when there were too many columns supplied to a rebdsim analysis configuration
  input text file.
* Fix a bug where the PrimaryFirstHit or PrimayrLastHit S coordinate may appear to jump back and forth
  or be discontinuous or wrong. This was fixed by using a more robust directional lookup in the geometry
  on boundaries. Although with the exact same coordinates, Geant4's navigation internally can 'stick'
  to surfaces and it's more robust to use a navigator search with a direction of motion included. For
  the primary trajectory we did a repeated point-only lookup, leading to occasionally the calculated S
  position from the centre of the element being wrong. Even if the primary trajectory isn't stored, a
  light version is used to identify the primary first and last hit points. This only happened in very
  specific circumstances and depended on the physics list used.
* Fix for incorrect curvilinear transforms resulting in wrong S coordinate. This was caused when the
  geometry search fell back to the curvilinear bridge world instead of the regular curvilinear world.
  The transform was used from the regular curvilinear world though, which would be the transform from
  the last lookup. This only affected a small fraction of cases with steps on boundaries on samplers in
  between elements. Most tracking routines do not depend on S / z, so there is little effect to tracking.
* Fix for field map rotation when using a tilt in the field. If the field was tilted by a multiple of
  :math:`\pi/2`, you would not notice. For small finite tilts, the field vector would be rotated wrongly
  due to a double transform.
* Fix a bug where the local coordinates of PrimaryFirstHit and PrimaryLastHit were always zero.
* Fix a bug where the turn number of PrimaryFirstHit and PrimaryLastHit was always zero.
* Fix sampler variables `theta`, `phi` and `phip` being -1 when it should be 0 for 0 angle particles
  due to a mistake in the identification of possible nans or infinite numbers.
* Fix check that the RF cavity horizontalWidth is larger than the cavity model radius when a cavity model
  is specified for that element.
* Correctly identify primary first hits on wire scanner wires. Due to the often very thin geometric
  nature of wires, a step through the wire is usually defined by transportation and not by a discrete
  physics process. However, the kinetic energy and momentum direction often change due to along-step
  processes that are not identified easily in Geant4. We now detect these changes and correctly identify
  the primary as impacting the wire as the PrimaryFirstHit location.
* Fixed a bug where the terminator and teleporters would overlap with the tunnel.
* Fixed two sources of overlaps which may appear when using `lhcleft` or `lhcright` magnet geometries.
* Fixed a bug where the `lhcright` transverse extent was set incorrectly.
* Placements with respect to thin multipoles would not work. Thin multipoles were always made uniquely
  where sometimes they didn't have to be - this has been fixed. Also, the searching algorithm has been
  improved to deal with any uniquely built components, such as rf cavities.
* Small memory leaks reported by Coverity.
* Unintialised variables reported by Coverity.
* Fix erroneous warnings with jcol that would prevent it being built. These were due to double
  parameter checks from a base class that don't appy.
* Fix Event.Summary.primaryAbsorbedInCollimator flag not identifying absorption in jcols correctly.
* Fix naming of placements so multiple placements of the same geometry are uniquely shown in the visualiser.
* Fix for test in `shield` element where the beam pipe wasn't built because it was compared to half the `xsize`
  instead of all of it. The beam pipe thickness was also not taken into account and now is.
* Fix potential overlap with octagonal beam pipes caused by incorrect determination of the radius
  required for the magnet poles to not hit the beam pipe.
* Fixed naming bug in magnets where the beam pipe container, magnet outer container and overall container
  logical volumes would have the same name. This would cause problems when exporting BDSIM geometry to
  GDML and then trying to reload it somewhere. Each are now named uniquely.
* Fix potential compilation problem with some compilers for "ambiguous overload of abs".
* Fix bug where `distrFile` executable option would not print out if set at the start of BDSIM.
* Fix print out for biasing that would incorrectly say "all particles" for biasing primary particles only.
  The message has also changed so as not to be confused with particle species.
* Fix the extension of any list type parameters in beam line elements when they're extended or redefined -
  such as updating the `knl` parameter of a multipole. Previously the parser would not understand this syntax.
* Fix survey writing for models with placement beam lines to now write those beam lines in separate files
  named as the survey name appended with the placement name. Previously the survey file was overwritten for
  every secondary beam lines so only the final beam line placement was recorded.
* Fixed parallel transport integrator for non-paraxial particles (e.g. secondaries from elsewhere) that would
  be parallel transported to the end of the element regardless of particle entry position or direction of travel.
  Non-paraxial particles are now tracked through as if the element were a drift. In the case of rmatrix elements,
  this change does not affect the behaviour of the rmatrix in the centre of the element, only the parallel transport
  through the thick sections of the element.
* Fix segfault in rebdsimOptics when supplying a BDSIM root file in which only primaries are generated, the model
  isn't constructed in this case so it isn't written, therefore can't be copied to the rebdsimOptics output.
* Fix wrongly sized container volume for ggmad geometry for Cons and Tubs solids as well as reported extents that
  would cause overlaps with neighbouring elements.
* Fix crash from Geant4 when the same sequence was placed multiple times (multiple beam line visualisation) due
  to degenerate naming of parallel worlds.
* Fix segfault in rebdsimOptics when the output file name is the same as the input file name. The two files names
  must now be different.
* Fix potentially bad geometry being built with exceptionally tightly bent dipoles with a short length. The
  check on length, angle and horizontalWidth was symmetric whereas for C-shaped poled dipoles the yoke can
  be shifted.
* Fix a bug where if the :code:`samplerDiameter` option was made incredibly small, the linked curvilinear
  volumes would also be shrunk and therefore result in a lack of transforms in incorrect fields and therefore
  tracking. The size of curvilinear world cylinders for field transforms is now determined independently.
* Fix possible overlaps reported in curvilinear transform volumes when a beam line with very strong bends
  is used. The volumes are built with more tolerance and also with a look behind previous in the beam line
  to avoid large volumes inbetween bends that migh overlap in a sequence of bends.
* `rcol` no longer warns about the entrance and exit x-y ratio to be the same (only ecol does), which had no effect.


Output Changes
--------------

* In the output, `Event.Trajectory.trajectories` is now `Event.Trajectory.XYZ` to better reflect
  what it is.  Similarly, `momenta` is now `PXPYPZ`. Capitals denote the global coordinates.
* The default behaviour with `option, storeTrajectories=1;` is now to **only** store the primary
  trajectory whereas it was all before. This vastly reduces the data size.
* The default option :code:`storeTrajectoryDepth` is now 0, representing only the primary whereas
  this was 1e5 before. -1 will mean 'all'. This in effect fixes a misunderstanding where trajectory
  options would not appear to have any effect unless the depth was set to 0.
* A new data member "filters" has been added to the Trajectory branch of the Event tree. This has
  bits (std::bitset<N>) that are 1 or 0 representing whether an individual trajectory matched each
  filter. This allows a mix of trajectories to be disentangled.
* In the analysis class :code:`analysis/Run.hh`, the member variables `Summary` and `Histos`
  now start with capital letters to match the layout on file.
* Samplers now have a new variable called `nElectrons` that is the number of electrons on a
  partially stripped ion (if it is one) passing through the sampler. This is filled alongside
  the other ion information.
* Samplers now have a new variable called `theta` included in polar coordinates (optional), which
  is the angle with respect to the local z axis. i.e. :math:`tan^{-1}(r^{\prime}/z^{\prime})`.
* `isIon`, `ionA` and `ionZ` are now non-zero when a Hydrogen ion with one or two electrons
  passes through a sampler.
* All extra coordinates are now recorded in the Primary sampler structure no matter if these
  are turned on or not for the samplers.
* New Event.Summary variable `cpuTime`, which is the duration of the event in CPU time in seconds.
* `e1`, `e2`, `hgap`, `fint`, `fintx`, `fintk2`, `fintxk2` variables in Model tree are now filled
  correctly.
* BDSOutputROOTEventCoords member variables are now all vectors instead of single numbers. This
  is to allow the possibility of more than one primary particle as is possible when loading a
  file from an event generator.
* New BDSOutputROOTEventAperture class.
* Consistency on `isIon` behaviour. A proton is not an ion, but a proton with bound electrons is.
* The variable :code:`duration` in Event.Summary and Run.Summary is now :code:`durationWall` to more
  accurately reflect the difference between this and the new variable :code:`durationCPU` for CPU time.
* The header class BDSOutputROOTEventHeader now has variables that store which files were analysed
  in the case of rebdsim and which files were combined in the case of rebdsimCombine.
* New variable :code:`nTracks` in Event.Summary which is the number of tracks created in that event.

Output Class Versions
---------------------

* Data Version 5.

+-----------------------------------+-------------+-----------------+-----------------+
| **Class**                         | **Changed** | **Old Version** | **New Version** |
+===================================+=============+=================+=================+
| BDSOutputROOTEventAperture        | Y           | NA              | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventBeam            | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCoords          | Y           | 1               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimator      | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimatorInfo  | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHeader          | Y           | 2               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHistograms      | Y           | 2               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventInfo            | Y           | 4               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLoss            | N           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLossWorld       | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventModel           | N           | 4               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventOptions         | Y           | 4               | 5               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventRunInfo         | Y           | 2               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSampler         | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectory      | Y           | 2               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectoryPoint | Y           | 2               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTGeant4Data           | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+

Utilities
---------

* pybdsim v2.2.0
* pymadx v1.8.0
* pymad8 v1.6.0
* pytransport v1.4.0


V1.3.3 - 2019 / 05 / 21
=======================

Bug Fixes
---------

* Hot fix for fields not attached to thin elements such as dipole fringes or thin multipoles. This bug
  crept in through a modification to avoid Geant4 getting stuck with strong fields in very narrow gaps
  between layers of geometry in beam pipes, resulting in subsequent bad tracking due to the bad state of
  Geant4 navigators internally. Regression testing has subsequently been introduced to protect against
  this kind of bugging going unnoticed in future.

V1.3.2 - 2019 / 04 / 20
=======================

New Features
------------

* Can now use any particle available through the physics list for a beam particle.
* Generic cross-section biasing can now be used for any particle, including ions.

General
-------

* Tested with Geant4.10.5.p01
* Geometry navigators are reset at the beginning of a run now in a similar way to the start of
  an event to ensure independence between runs - future proofing.
* For Geant4.10.5, we now use the 'low' looping particle thresholds for tracking.
* The 'vacuum' field is now not applied to the container volume of a beam pipe. However, it is
  still applied to the vacuum and beam pipe volumes. This makes the tracking more robust against
  stuck particles in the extremely small gap between volumes.
* The yoke magnetic field now uses a wrapped G4ClassicalRK4 integrator. This wrapper acts as
  a drift for short (< 1um) steps. This makes tracking more robust for secondaries in the yoke.
* Improve testing for user bunch distribution for robustness.
* Increase transverse length safety margin between beam pipes and magnet volumes for safety.
* Translate bunch coordinates in global coordinates backwards by 1x length safety to avoid
  starting on a volume boundary at the start of the event. This is 1nm so will not affect
  tracking results. The local coordinates in the output are identically the same.

Bug Fixes
---------

* Fix strong recreation when using user file supplied bunch distribution. The file was
  always read from the beginning in the past. Now the correct coordinates will be
  read and the event is correctly reproduced.
* Fix userinterface example given changes to sensitive detector manager - simple edit.
* Fix calculated phase offset for rfcavity in the beam line. This was peak at the
  end of the element rather at the middle.
* Fix possible segfault if event aborted due to extra collimator hit information.
* Fix user file beam loader for anomalous particle coordinates generated once at the end
  of a file if the end of the file was a blank line. It would result in all zero coordinates
  and beam energy x1000 for the first particle generated after the end of the file was reached.
* Fix abort of event if unknown particle ID specified in user file beam loader.
* Fix user file distribution file loading for comment lines, incomplete lines and empty
  (white space) lines.
* Fix phase offset calculation for rf cavities with respect to nominal value. Phase would have
  been smaller than intended. It was scaled to :math:`1/2\pi` instead of :math:`2\pi`.
* Fix ambiguity in manual for rf cavities. Time is generally in seconds in BDSIM, however the
  rf cavity took nanoseconds. A time offset of `1*ns` in the input gmad would result in double
  units.
* Fix warning when loading an output file with data loader class when the file was created
  without storing primary coordinates. The warning was related to the PrimaryGlobal branch.
* Fix warnings and artificial killing of particles by high looping particle thresholds for
  Geant4.10.5, which are default. Use the 'low' looping thresholds by default. Issue #268.
* Fix stuck particles by attaching the vacuum field in a beam pipe to every volume in the
  beam pipe apart from the container volume to avoid navigation problems in very thin gaps. Issue #268.
* Remove half-implemented integrator types in internal dictionaries.
* Fixed model-model example conversion Python scripts as these were specific to the developer's computer.
* Fix coil end-piece placement with respect to main magnet body - now includes required length safety
  gap to avoid possible navigation issues with large sized models.
* Fix for exotic particle beams. Can now use any particle available in the physics list.
  Particle definitions now constructed earlier than in the regular physics list call.
* Fix bad tracking in undulators caused by nan values in field caused by querying the field
  at arbitrarily large positions. Limited the range of validity of the field to the beam pipe. The
  field is now no longer attached to the magnets outside the beam pipe.
* Biasing was not attached to components that were found to be unique in construction - i.e. an
  rbend back-to-back with another rbend will not have fringe fields in the middle at the join, so
  is considered a unique construction. This would result in these not having biasing attached.

Utilities
---------

* pybdsim v2.1.0
* pymadx v1.7.1
* pymad8 v1.5.0
* pytransport v1.3.0


V1.3.1 - 2019 / 03 / 05
=======================

Bug Fixes
---------

* Fix transform3d element where offsets were not working.


V1.3 - 2019 / 02 / 27
=====================

Expected Changes To Results
---------------------------

* The density of the surrounding air has changed very slightly to that of the standard
  G4_AIR one.
* Energy deposition in vacuum is now separated into a separate branch and is not mixed
  with general Eloss. Therefore, less energy deposition will be seen in the Eloss branch.
* The minimum kinetic energy option will now be respected when using a Geant4 reference
  physics list, whereas it wasn't before.
* The range cuts can now be applied when using a Geant4 reference physics list if the
  option :code:`g4PhysicsUseBDSIMRangeCuts` is turned on (=1). Previously, these had no
  effect with a Geant4 reference physics list.

New Features
------------

* Support for Geant4.10.5.
* New environment script in :code:`<bdsim-install-dir>/bin/bdsim.sh` to make running BDSIM easier.
* All Geant4 reference physics lists are now available.
* New beam pipe aperture for the CLIC post collision line.
* New jaw collimator element "jcol" with two blocks in the horizontal plane.
* New wire scanner element "wirescanner" with cylindrical wire in a beam pipe.
* Completed CMake to allow user applications based on BDSIM to easily link against it.
* New :code:`dump` beam line element that is an infinite absorber. This prevents simulations
  running for a long time when particles may hit the air at the end of the beam line.
* BDSIM as a class for interfacing. Ability to add custom beam line components.
  See :ref:`interfacing-section`.
* New samplerplacement object that defines an arbitrarily placed sampler in the world that
  may overlap with anything (see :ref:`user-sampler-placement`).
* New importance sampling implementation when using a user-supplied world geometry. (see
  :ref:`physics-bias-importance-sampling`.

* New options:

.. tabularcolumns:: |p{0.30\textwidth}|p{0.70\textwidth}|
  
+----------------------------------+------------------------------------------------------------------+
| **Option**                       | **Description**                                                  |
+==================================+==================================================================+
| collimatorsAreInfiniteAbosrbers  | When turned on, all particles that enter the material of a       |
|                                  | collimator (`rcol`, `ecol` and `jcol`) are killed and the energy |
|                                  | recorded as deposited there.                                     |
+----------------------------------+------------------------------------------------------------------+
| geant4Macro                      | Fun an optional macro in the visualiser once it's started.       |
+----------------------------------+------------------------------------------------------------------+
| g4PhysicsUseBDSIMCutsAndLimits   | If on, the maximum step length will be limited to 110% of the    |
|                                  | component length - this makes the tracking more robust and is    |
|                                  | the default with a regular BDSIM physics list. The minimum       |
|                                  | kinetic option is also obeyed. Default off.                      |
+----------------------------------+------------------------------------------------------------------+
| g4PhysicsUseBDSIMRangeCuts       | If on, this will apply the BDSIM range cut lengths to the Geant4 |
|                                  | physics list used. This is off by default.                       |
+----------------------------------+------------------------------------------------------------------+
| ignoreLocalMagnetGeometry        | If turned on, this option means that only the magnet geometry    |
|                                  | from options will be used. Similar to `ignoreLocalAperture`.     |
+----------------------------------+------------------------------------------------------------------+
| importanceVolumeMap              | File path for text file that maps importance values to volumes.  |
+----------------------------------+------------------------------------------------------------------+
| importanceWorldGeometryFile      | File path for the externally provided geometry that will be used |
|                                  | as the parallel world for the importance sampling.               |
+----------------------------------+------------------------------------------------------------------+
| physicsEnergyLimitLow            | Control minimum energy for all physics models. (advanced)        |
+----------------------------------+------------------------------------------------------------------+
| physicsEnergyLimitHigh           | Control maximum energy for all physics models. (advanced)        |
+----------------------------------+------------------------------------------------------------------+
| minimumKineticEnergyTunnel       | Any particles below this energy (in GeV by default) will be      |
|                                  | artificially killed in all BDSIM-generated tunnel segments.      |
+----------------------------------+------------------------------------------------------------------+
| storeCollimatorInfo              | Store collimator structure with primary hits per collimator.     |
+----------------------------------+------------------------------------------------------------------+
| storeCollimatorHitsAll           | If `storeCollimatorInfo` is on and collimator hits are           |
|                                  | generated, hits will be generated for all particles interacting  |
|                                  | with the collimators whether primary or secondary and whether    |
|                                  | ion or not.                                                      |
+----------------------------------+------------------------------------------------------------------+
| storeCollimatorHitsIons          | If `storeCollimatorInfo` is on and collimator hits are           |
|                                  | generated, `isIon`, `ionA` and `ionZ` variables are filled.      |
|                                  | Collimator hits will now also be generated for all ions.         |
+----------------------------------+------------------------------------------------------------------+
| storeCollimatorHitsLinks         | If `storeCollimatorInfo` is on and collimator hits are           |
|                                  | generated, extra information is stored for each collimator hit.  |
+----------------------------------+------------------------------------------------------------------+
| storeEloss                       | Ability to completely turn off generation of energy deposition   |
|                                  | hits to save memory usage and output file size. Default on.      |
+----------------------------------+------------------------------------------------------------------+
| storeElossModelID                | Control whether the beam line index is stored in the energy      |
|                                  | loss output. More granular than :code:`storeElossLinks`.         |
+----------------------------------+------------------------------------------------------------------+
| storeElossTurn                   | Control whether energy deposition turn number is saved.          |
+----------------------------------+------------------------------------------------------------------+
| storeElossVacuum                 | Control whether energy deposition in the residual gas in the     |
|                                  | beam pipe 'vacuum' is recorded.                                  |
+----------------------------------+------------------------------------------------------------------+
| storeElossWorld                  | Turn on generation of energy deposition in the world volume      |
|                                  | (i.e. the air) as well as record energy leaving the simulation.  |
|                                  | Default off.                                                     |
+----------------------------------+------------------------------------------------------------------+
| storeElossWorldContents          | Turn on generation and storage of energy deposition in any       |
|                                  | included with the externally provided world geometry. Off by     |
|                                  | default but turned on automatically when using importance        |
|                                  | sampling. Allows the user to distinguish energy deposition in    |
|                                  | the air as stored in ElossWorld from the contents of the world.  |
+----------------------------------+------------------------------------------------------------------+
| storeSamplerAll                  | Conveniently store all optional sampler data with one option.    |
+----------------------------------+------------------------------------------------------------------+
| storeSamplerKineticEnergy        | Store kinetic energy in the sampler output.                      |
+----------------------------------+------------------------------------------------------------------+
| storeSamplerPolarCoords          | Store the polar coordinates (r, phi and rp, phip) in the         |
|                                  | sampler output.                                                  |
+----------------------------------+------------------------------------------------------------------+
| tunnelIsInfiniteAbsorber         | When turned on, any BDSIM-generated tunnel segments will absorb  |
|                                  | and kill any particle of any energy. Used to speed up the        |
|                                  | simulation. Default off.                                         |
+----------------------------------+------------------------------------------------------------------+
| worldGeometryFile                | External geometry file for world geometry.                       |
+----------------------------------+------------------------------------------------------------------+

* Access to data version in DataLoader in analysis.
* External geometry can be supplied as the world volume with the option
  :code:`worldGeometryFile`.
* New complete physics list for crystal channelling to achieve the correct result.
* New ability to specify a different beam particle that is different from the design
  particle used for magnetic field strength calculations (:code:`beamParticleName`).
* Specify the particle assumed for the user file distribution that can be different from
  the design particle.
* New option to use a one turn map generated from MAD-X PTC to correct
  multi-turn tracking for circular machines.
* New option :code:`geant4Macro` and executable option :code:`--geant4Macro` to run an optional
  macro in the visualiser once it's started.
* A warning will print if a user-defined material is more dense than 100g/cm3 as this is much higher
  than any naturally occurring material (on Earth). The simulation will still proceed.
* New optional collimator output structure in event made per collimator with prefix
  "COLL\_". Controlled by new option :code:`collimatorInfo`.
* New mini-summary of collimators in Model tree when :code:`collimatorInfo` option is used.
* New parameter for collimator elements :code:`minimumKineticEnergy` that allows the user to kill
  particles below a certain kinetic energy in a collimator.

General
-------

* All collimators now require a material to be specified and the default copper has
  been removed. This is because it strongly affects the results obtained and defaults
  should not be relied upon for this.
* The turn number for energy deposition hits is now automatically stored if
  a circular model is used.
* The `sensitiveBeamlineComponents` option has now been renamed to `sensitiveOuter`
  to better reflect its functionality. The old option is still accepted.
* The `tunnelSensitive` option has now been renamed to `storeElossTunnel` to be
  more consistent with the other sensitivity options. The old option is still
  accepted.
* The generic beam line element `element` now supports angle and the beam line
  will be curved by this amount.
* The world volume is now sensitive and can record energy deposition. Geant4.10.3 upwards
  is required to record both this information and the energy leaving the world
  as this requires G4MultiSensitiveDetector.
* New tests for testing backwards compatibility of analysis tool with previous data version.
* "Model Preparation" is now "Model Conversion" in the manual to be clearer.
* Visualisation now uses macro search path to look for visualisation macro in the installation
  directory then the build directory of BDSIM.
* In recreate mode, there is explicit print out about when the seed is set and if if was successfully
  loaded from the output file.
* The Cherenkov example has now been updated to show 3 materials (air, water, YAG).
* Fixes from static code analysis for virtual functions called in constructors of factories,
  shadow member variables and initialisation of crystal variables in parser.
* Significant reduction in use of the singleton pattern for beam pipe, magnet yoke,
  tunnel and geometry factories.
* Reduced memory usage for energy deposition hits by removing unused numbers stored each time.
* Reduced memory usage for energy deposition hits when not using extra variables such as the 'links'.


Materials
---------

* The materials construction in src/BDSMaterials.cc was checked through thoroughly.
* "air" is now G4_AIR instead of custom BDSIM air (similar composition). The old air is now "airbdsim".
* The refractive index data for optical and cherenkov physics has been added on top of G4_AIR
  as well as "airbdsim".
* "airbdsim" now has a density of 1.225mg/cm3.
* "bp_carbonmonoxide" material now has correct pressure (previously near infinite).
* Fixed double density for the following materials. They would have been extremely dense.
  
   - "berylliumcopper"
   - "stainless_steel_304L"
   - "stainless_steel_304L_87K"
   - "stainless_steel_316LN"
   - "stainless_steel_316LN_87K"
   - "tungsten_heavy_alloy"
   - "fusedsilica"
   - "n-bk7"
   - "yag"
   - "pet"
   - "lhc_rock"

* "niobium" is now "niobium_2k" to better reflect the unusual temperature.
* "nbti" is now "nbti_4k" to better reflect the unusual temperature.
* "waterCkov" has been removed. "water" or "G4_WATER" (the same) should be used. The refractive
  index data has been added to G4_WATER material.

Developer Changes
-----------------

* The BDSGeometryComponent base class now has the ability to specify which
  sensitive detector should be attached in a map using the BDSSDType enum. There is no default
  sensitive detector (previously general energy deposition) as the developer must be explicit
  about what sensitivity they want so nothing unexpected can happen.
* BDSBeamline can now return indices of beam line elements of a certain type.
* All sensitive detector classes have been renamed as have the accessor functions in BDSSDManager.
  This is to make the naming more consistent.

Bug Fixes
---------

* Fixed reloading user file when reading more particles than defined in the file.
* Fixed "pt" column in user file for reading particle PDG IDs. The first particle would be read
  correctly and all subsequent particles would revert to the beam definition.
* Fixed infinite tracking from nans return from field map when BDSIM format
  field map file was lacking lower and upper limits.
* Fixed incorrect writing of optional sampler information.
* The `sensitiveBeamPipe` option now works and controls whether the beam pipe produces
  energy loss or not. This does not affect the physics, merely whether output
  information is generated or not.
* The `sensitiveOuter` (formerly `sensitiveBeamlineComponents`) option has
  been fixed and now controls whether the parts outside the beam pipe in an
  element record energy loss or not.
* Degrader and undulator did not record energy deposition.
* Energy deposition is now correctly recorded when tracks are artificially killed.
* Fix crystal channelling with cylindrical and torus shaped crystals. The crystal implementation
  only works along the local X direction of any solid. Fixed by using a G4DisplacedSolid to
  allow use of more advanced geometries than a box.
* Fix channelling physics for standard EM and hadronic processes as this requires process biasing.
* Fix A and Z being the wrong way around for ions in samplers.
* Charge now correctly recorded in primaries and in samplers for partially stripped ions.
* Solenoid tracking fixed. Fringes are constructed as appropriate according to integrator set.
* Fix possible nan values given to Geant4 tracking with miscalculated auto-scaling value for
  field maps.
* Fix setting default seed state for random number generator if using recreate mode
  and progressing beyond an event stored in the file.
* Fix setting the energy level of an ion - wasn't set from input.
* SQL geometry factory didn't clean up after repeated use. This geometry isn't
  generally supported.
* Fixed a bug where very weak actions on particles in tracking would not be taken due to
  too stringent tests of finite numbers. This would result in particles with small offsets
  in magnets or particles with high momentum that would see only very small deviations being
  tracked as if it were a drift.
* Fixed segfault crash from ROOT with rebdsim when there were more dimensions in the variables
  than the declared number of dimensions. For example, "y:x" for Histogram1D.
* Fixed rare bug where segfault would occur in trying to account for energy deposition of
  artificially killed particles.
* Fix memory leak of sampler structures (relatively small).
* Fixed parsing of + or - symbols with ion definition. Now supports H- ion.
* Fixed very slow memory leak associated with the primary trajectory. only visible for very
  large numbers of events.
* Fixed dipole tracking for off-charge ions - reverts to backup integrator.
* Fixed Pythonic range iteration of Event tree when trying to look at Info branch. Conflicted with
  Info method of TObject. Now renamed to Summary.
* Fixed catching the construction of dipoles with too large an angle. Limit rbends and unsplit
  sbends to a maximum angle of pi/2, limit the maximum angle of all other dipoles to 2 pi.

Output Changes
--------------

* "Info" branch of the Event and Run trees are now "Summary". This is to avoid conflict with
  ROOT TObject::Info() that could result in broken analysis or range iteration. The DataLoader
  class in analysis (used by pybdsim.Data.Load) is backwards compatible. In the case of loading
  older data with updated software, there will still be a member called Info that the data will
  be loaded into. Python range iteration cannot be used in this case.
* "TunnelHit" is now "EnergyLossTunnel" to be consistent. `rebdsim` and the analysis DataLoader
  class (both Python and ROOT) are backwards compatible and both TunnelHit and ElossTunnel are
  available. Only the correct one is filled with loaded data during analysis.
* Much more granular control of what is stored in the output. See new options in 'new' section
  above.
* Vacuum energy deposition separated from general energy deposition and now in its own branch.
* Memory usage (for Mac & Linux) added at the end of each event in event info. This
  is the memory usage of the whole program at that point including event independent
  quantities such as the model.
* Boolean flag store in even info as to whether the primary was absorbed in a collimator or not.
* New options to control level of output as described in table in new features..
* Tunnel energy deposition hits now respond to the :code:`storeElossXXXX` options to control the
  level of detail with extra variables of their output.
* New class BDSOutputROOTEventLossWorld for a record of coordinates when a particle leaves a volume,
  use currently for exiting the world.
* New structures ("branches") in the `Event` tree called :code:`ElossWorld` and
  :code:`ElossWorldExit` for energy deposition in the world material and energy leaving
  the world (and therefore the simulation) respectively.
* New members in :code:`Event.Info` that are the integrated energy deposited in various parts
  for that event. These are for convenience and are the integrals of the various Eloss parts.

Output Class Versions
---------------------

* Data Version 4.

+-----------------------------------+-------------+-----------------+-----------------+
| **Class**                         | **Changed** | **Old Version** | **New Version** |
+===================================+=============+=================+=================+
| BDSOutputROOTEventBeam            | Y           | 2               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCoords          | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimator      | Y           | NA              | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimatorInfo  | Y           | NA              | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLossWorld       | Y           | NA              | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHeader          | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHistograms      | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventInfo            | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLoss            | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventModel           | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventOptions         | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventRunInfo         | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSampler         | Y           | 2               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectory      | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectoryPoint | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTGeant4Data           | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+


Utilities
---------

* pybdsim v2.0.0
* pymadx v1.7.0
* pymad8 v1.5.0
* pytransport v1.3.0


V1.2 - 2018 / 08 / 26
=====================

Highlights
----------

* :code:`outerDiameter` is now :code:`horizontalWidth` to better describe its function (backwards-compatible).
* Fixed dipole scaling with (the default) bdsimmatrix integrator set.
* Solenoid tracking fixed.

New Features
------------

* New options to activate extra models in em_extra physics list.
* New :code:`crystalcol` element for channelling crystals.
* New :code:`crystal` definition in parser.
* New "channelling" physics list for Geant4 crystal channelling physics process.
* Field maps need not be in `x`, `y`, `z`, `t` order and lower dimension fields (i.e. 1D or 2D) can
  be made for any dimension, i.e. it is now possible to specify a 1D field along the `z` direction.
* Rebdsim can now analyse a select range of events specified by "EventStart" and "EventEnd" options.
  Issue #240.
* Placements can now be made with respect to S,x, and y in the main beam line, with respect to a beam line
  element and lastly in global Cartesian coordinates.
* Samplers will no longer be automatically attached (with :code:`sample, all;`) to dipoles with finite
  pole face rotations, as this may lead to tracking issues in Geant4. A developer option can force
  this on, although this is not recommended. Issue #241.
* `hkicker` and `vkicker` strength can be specified via the magnetic field :code:`B` instead of
  `hkick` or `vkick`.
* Support for dipole poleface curvature in tracking.
* Pole face rotations and fringe fields are now available for hkickers and vkickers, both thick and thin.
* New ability to specify the colour of any magnet and most elements through custom colour definition.
* Geant4's DNA physics lists have been added.
* Solenoid fringe fields have been implemented and are on by default. They are controlled with
  the `includeFringeFields` option.
  
General
-------

* :code:`outerDiameter` is now :code:`horizontalWidth` to better describe its function
  (backwards-compatible). This naming was from a time when BDSIM could only create cylindrical
  magnets and beam pipes. Given it can now create more complicated geometry, this name is not
  a good choice and so has been renamed. BDSIM still supports the old syntax.
* :code:`vhratio` is now consistent with vkickers and refers to the vertical and horizontal ratio in
  the lab frame.
* The horizontal width of kickers is now taken from :code:`outerDiameter`. Previously, :code:`outerDiameter`
  corresponded to the height and :code:`vhratio` was really the horizontal-to-vertical ratio in
  the lab frame.
* Synchrotron radiation is now disabled with em_extra physics list (use dedicated
  synchrad physics list). Avoids the double registration of the physics process.
* New CMake variable ROOTSYS to allow easy specification of a specific ROOT installation.
* Visualisation of trajectories significantly faster (~10x) due to different strategy with Geant4
  visualisation system.
* "ang" unit is added to the parser for Angstroms.
* BDSIM will now exit if there is no space to make the necessary circular management objects
  (teleporter and terminator).
* long int used explicitly instead of int for event indices in analysis.
* Reimplemented primary first hit and last hit. Last hit is now the end point of the
  primary trajectory. No more linear memory usage with tracking time.
* Beam pipe extent calculation re-implemented and much less simplistic - used
  to check whether a pipe will fit inside a magnet.
* Mini-contents for syntax section of manual, as it's grown to a large size.
* New rmatrix element (experimental).
* EM Dissociation is now applicable up to 100 TeV.
* Significantly improved aperture shape checking for whether beam pipe will fit inside a magnet.
* BDSIM now recognises all elements by chemical abbreviation. These are found in the Geant4 NIST
  database by automatically prefixing the name with "G4\_". Issue #236.
* `circle` distribution z and t distributions are now reversed to be consistent with all other generators.
  These were T0 - dt and are now T0 + dt. Distribution will be different for the same seed as compared
  to a previous version of BDSIM.
* `square` distribution now calls random number generator for each coordinate every time for
  consistency. Distribution will be different for the same seed as compared
  to a previous version of BDSIM.
* Memory usage for sampler hits has been significantly reduced with no affect to the output
  information stored.
* The "water" material in BDSIM is now the NIST G4_WATER material and no longer the one
  that was defined by BDSIM.
* New options for physics processes in em_extra.

Output Changes
--------------

* Data v3 incremented from v2.
* Options class (GMAD::optionsBase) number is incremented in output.
* New optional stepLength variable in Eloss part of Event Tree with option
  :code:`storeElossStepLength` to use this data.
* New optional preStepKineticEnergy in Eloss part of Event Tree with option
  :code:`storeElossPreStepKineticEnergy` to use this data.
* Energy Loss class (BDSOutputROOTEventLoss) number is increment in output.
* Tilt, offsetX, offsetY and material are added to the Model Tree output.
* Model class (BDSOutputROOTEventModel) number is incremented in output.
* Model information extended to provide everything necessary to make machine diagrams.
* New option :code:`storeModel` to turn off model storage in the output.
* Even Info class (BDSOutputROOTEventInfo) number is incremented in output.
* Event.Info now has a Boolean of whether the primary particle hit something or not.
* Samplers are **no longer** placed next to elements with angled faces when using the :code:`bdsimtwo`,
  :code:`geant4`, or :code:`geant4dp` integrator sets.
* Units are now written to the ASCII survey output for each column.
* New output class :code:`BDSOutputROOTEventCoords` to store coordinates for primary global coordinates.
* New branch called "PrimaryGlobal" in Event tree that stores the coordinates used with Geant4 in
  the global Cartesian frame.
* Sampler name now stored in Orbit output from rebdsimOrbit to make sampler matching possible.

Bug Fixes
---------

* Fixed solenoid tracking. The anti-spiralling code in the dipole integrator that is designed
  to stop infinite spiralling of low energy particles in strong fields was causing incorrect
  tracking in solenoids. This has been fixed with the re-implementation of the solenoid matrix
  and now includes the fringe effects. Issue #255.
* Fixed tracking bug where particle in very niche coordinates may reflect from a sampler
  at the end of a dipole with a very strongly angled pole face. #Issue 241.
* Fixed automatic tunnel building algorithm, which accumulated wrong variables, leading to
  problems when thin elements such as fringe fields or thin multipoles were included.
* Further improvements made to tunnel building algorithm for magnets with tilt. Issue #243.
* Fixed length check for very short elements. Small drifts would cause a crash from
  Geant4 due to invalid parameters - occurred as length check was wrong.
* Fixed non-positive definite warnings for no energy spread and time spread when using
  a Gaussian beam in a composite beam definition.
* Fixed Gauss beams used in composite distribution.
* Fixed the problem where no particles were being tracked when using a userfile bunch distribution with only one column.
* Fixed bug where last particle was missed from user bunch distribution file.
* Fixed corrupted example files for userfile bunch distribution. Issue #244.
* Fixed cutting planes in G4CutTubs constructor for tunnel in Geant up to Geant4.10.2.p02
  from too short tunnel section.
* Reimplemented the method of finding primary first and last hit so BDSIM doesn't need to
  save the whole trajectory for the primary. This fixes the behaviour of linearly growing
  unbounded memory usage when tracking for a long time in a ring. Issue #246, #242.
* Optical calculation now works for sub-relativistic positrons.
* ATF2 MAD-X output was not included in worked example as advertised - now included.
* Fixed scaling variable used when scaling a field map to a decapole magnet strength.
* Survey units for magnetic fields are now fixed from kT to T.
* Fixed issue where C-shaped vkickers and hkickers would ignore :code:`yokeOnInside`. Issue #251.
* Fixed possible overlap in vkicker, hkicker, and h-style dipole geometry with highly asymmetric
  beam pipes.
* Fixed incorrect report that beam pipe wouldn't fit in magnet for various aperture shapes. Issue #253.
* Fixed issue where the option :code:`storePrimaries = 0` would result in the hits for the first sampler
  being written to the primary sampler structure. Issue #245.
* Fixed lack of interaction with vacuum when processes biased - due to a specific Geant4 version.
  Issue #220.
* Fixed incorrect dipole scaling. Issue #239.
* Fixed E0 spread in `ring` beam distribution, which was finite in contrast to the description
  that it is always the central value.
* Fixed reproducibility for the `ring` distribution that didn't use the same random number generator
  as every other distribution. Coordinates will be different for this distribution for the same seed now.
* Fixed inconsistency of `t` and `z` coordinate in `square` beam distribution.
* `square` beam distribution now varies with :code:`envelopeT`.
* Fixed S coordinate in output. Issues #247 and #248.
* Fixed the setting of the sampler diameter where the user specifies a smaller one than that calcualted
  from the minimum bending radius.

Utilities
---------

* pybdsim v1.9.0
* pymadx v1.5.0
* pymad8 v1.4.1
* pytransport v1.2.1
  

V1.1 - 2018 / 05 / 23
=====================

New Features
------------

* New visualiser command "/bds/beamline/goto name" to move visualiser to view a particular element.
* Check explicitly on incompatible physics lists that may cause a crash when used together.

General
-------

* Global coordinates are now always stored for primary first hit.
  
Bug Fixes
---------

* Fixed wrong transforms for finite `S0` in composite beam distribution.
* Fixed crash when finite `S0` was used with `-\\-generatePrimariesOnly` executable option.
* Fixed units from mm to m for PrimaryFirstHit and PrimaryLastHit for
  `x`, `y`, `z`, `X`, `Y`, `Z` positions.
* Fixed segfault for double deletion when 'qgsp_bic' and 'qgsp_bert' were attempted to be used together.

Utilities
---------

* pybdsim v1.6
* pymadx v1.2
* pymad8 v1.1
* pytransport v1.0


V1.0 - 2018 / 04 / 10
=====================

Highlights
----------

* Full support for dipole fringe fields and pole faces in tracking.
* Full low energy (sub-relativistic) tracking.
* Validation against PTC for sub-relativistic to high energy.

New Features
------------

* Support for Geant4.10.4 - however, this version is unusable as G4ExtrudedSolid is
  broken and used in BDSIM. We recommend Geant4.10.4.p01.
* H-style dipoles controllable by default or per element with `hStyle` option.
* Control over dipole proportions with global and per element options `vhRatio`,
  `coilWidthFraction` and `coilHeightFraction`.
* Support for extra Geant4 physics lists: `G4ChargeExchangePhysics`, `G4HadronDElasticPhysics`,
  `G4HadronElasticPhysicsHP`, `G4HadronElasticPhysicsLEND`, `G4HadronElasticPhysicsXS`,
  `G4HadronHElasticPhysics`, `G4HadronPhysicsShielding`, `G4HadronPhysicsShieldingLEND`,
  `G4IonElasticPhysics`, `G4IonQMDPhysics`, `G4RadioactiveDecayPhysics`, `G4StoppingPhysics`,
  `G4HadronElasticPhysicsPHP`, `G4MuonicAtomDecayPhysics`, `G4NeutronTrackingCut`.
* New options `neutronTrackingTime`, `neutronKineticEnerygLimit` and `useLENDGammaNuclear`
  (Geant4.10.4 onward) physics options.
* Support for new numerical integrator tracking algorithms in Geant 4.10.3 and 4.10.4.
* New integrator set "geant4dp" for Dormand Prince integrators (Geant 4.10.4 or higher required).
* Significantly improved analysis documentation.
* New component: 'gap'. Creates a space in the beam line with no geometry.
* Ability to specify the world volume material with the `worldMaterial` option.
* Introduced `minimumRange` and `minimumKineticEnergy` user limits as provided by G4UserLimits.
* Ability to limit step size in user-specified fields.
* Ability to control turn number print out with `printFractionTurns`.
* Magnet yokes now have a general multipolar field.
* Sampler diameter is automatically reduced when high angle bends are used to prevent overlaps.
* New CMake option to disable event display for installation with ROOT EVE problems.
* Ability to combine rebdsim output files with correct statistical analysis, allowing high throughput
  analysis with scaling. New tool `rebdsimCombine` for this purpose.
* Parameter tests for all bunch distributions - BDSIM will exit with message if parameters are invalid.
* `scaling` parameter for each element allowing simple scaling of strengths.
* New program to convert PTC output in a TFS file to BDSIM output format (`ptc2bdsim`).

Output Changes
--------------

* The ROOT class definition has been incremented to "2" from "1".
* Output files now have header structure with software versions.
* Output files now have 'geant4 data' that includes particle masses used in the simulation.
* "t" is now "T" in samplers and trajectory output to be consistent with naming convention
  of global and local coordinates.
* Samplers now have optional charge, mass, rigidity and ion A and Z
  (see :ref:`bdsim-options-output` for details).
* Classes in library can calculate on-the-fly for user analysis.
* Trajectory momentum is now in GeV.

Analysis Changes
----------------

* "**librebdsimlib**" has been changed to "**librebdsim**" to be more consistent for output loading.
* Support for logarithmic binning of histograms in rebdsim.
* "HistogramND" in rebdsim now creates **per-entry** histograms on the tree. This introduces the
  ability to create per-event histograms in analysis that were not previously possible. Older
  style histograms that are a sum across all events are now made with "SimpleHistogramND".
* New option in rebdsim to turn off histogram merging (for speed).
* Analysis classes have member names changed to match those in the output files, i.e. "eloss" is
  now "Eloss" in `bdsim/analysis/Event.hh`.
* Rebdsim is now tolerant of a missing '.' on the end of the Tree name (a ROOT subtlety).
* 'orbit' and 'optics' are now 'Orbit' and 'Optics' in the output.
* New executable option for `rebdsimOptics` :code:`--emittanceOnFly` to calculate emittance
  at each sampler.

General
-------

* Physics list names are now consistently named with '_' between words. Old list
  names are still supported.
* `hadronic` and `hadronic_hp` physics lists have switched from `qgsp_bert` and
  `qgsp_bert_hp` to `ftfp_bert` and `ftfp_bert_hp` respectively, as these are
  recommended by Geant4 for high energy hadronic interactions.
* "bdsim" integrator set now maps to "bdsimtwo" integrator set.
* All objects in the parser can now be extended later rather than just elements.
* Tuned colours of hkicker and vkicker.
* Relative file paths are no longer padded excessively with slashes when translated
  to absolute paths.
* More efficient file IO in rebdsim improves analysis speed.
* The world material is now air instead of G4_Galactic.
* `printModuloFraction` is now `printFractionEvents`.
* `includeIronMagFields` option is now `yokeFields`.
* Vacuum volumes now visible in `-\\-vis_debug` mode.
* Only forward moving particles are now considered for the optical function calculation.
* Updated examples for ATF2, LHC. New Diamond machine example.

Bug Fixes
---------

* Fixed magnetic field strength for AWAKE dipole using pure dipole field.
* User limits are now applied to external geometry.
* Fixed bug where some visualisation settings wouldn't be applied to all logical
  volumes in external geometry.
* Fixed bug where some file paths may not be translated to absolute paths correctly.
* Fixed a bug where recreate mode would fail with the new Beam structure in the output.
* Prevent segfault when analysing wrong type of file with rebdsim (use 'backwardsCompatible'
  option to analyse old files without the new header structure).
* The `stopTracks` option has been removed as it did not function as intended. Use `stopSecondaries`.
* `thresholdCutCharged` and `thresholdCutPhotons` have been removed as they did not function as intended.
* Fixed bug where the world size would be too small with placements of large externally provided
  geometry with strong rotations.
* When no energy was specified in the `userfile` bunch distribution, the central beam energy was
  1000x too low - fixed - missing factory of GeV.
* Number of turns simulated is now number desired and not number+1.
* Limits to control maximum step length and tracking time were not attached to magnet yoke geometry.
* Fixed crash when using octagonal or racetrack apertures in a magnet with pole geometry.
* Fixed issue where ~10% of particles may appear offset in samplers in large ring models.
* Fixed some very small overlaps not reported by Geant4 in magnet yoke geometry.
* Fixed issue where drift in magnet was always 1pm shorter than expected.
* Fringe fields are no longer built in between two dipoles against each other.
* Fixed Gauss Twiss bunch generator for finite dispersion in `x`, `xp`, `y`, `yp`.
* Fixed bug where overlapping dipole end pieces would be produced.
* Fixed GDML preprocessing for parameterised variables.
* Tracking limits are now attached to magnet yokes.
* Fixed central value of `T0` not being set for `circle`, `gauss`,
  `gausstwiss`, `gaussmatrix`, `halo` and `square`
  distributions.

Utilities
---------

* pybdsim v1.4
* pymadx v1.1
* pymad8 v1.0
* pytransport v1.0

V0.993 - 2017 / 12 / 11
=======================

New Features
------------

* Executable option `-\\-writeseedstate` is now `-\\-writeSeedState`.
* Executable option `-\\-exportgeometryto` is now `-\\-exportGeometryTo`.
* Executable option `-\\-distrfile` is now `-\\-distrFile`.
* Redefined simpler syntax for halo distribution. Please see manual for new parameters.
* Support for all EM physics lists included with Geant4.10.3.p03.
* Support for an ion as the primary beam particle.
* Support for ion physics lists.
* Ability to load two GDML files with degenerate object names correctly compensating for
  deficiency in Geant4 GDML parser.
* Local copy of GDML schema included. No longer require internet access to load GDML files. Custom
  local schema still supported.
* Support for Geant4.10.4 - however, we don't recommend using this until p01 is used, as there
  is a fault with G4ExtrudedSolid that is used for all poled magnet geometry.

Bug Fixes
---------

* Compilation fixes for compilers with XCode 9.
* Fixed possible compiler-dependent tracking bug where particle would get stuck in dipole.
* Cherenkov radiation in water fixed with specification of optical refractive index.
* Fixed ATF2 example input syntax and updated model.
* Removed temporary files created in current working directory.

Output Changes
--------------

* Options split into options and beam - beam contains all information related to beam definition.
* Associated output classes and analysis classes added for Beam in output.
* Removed older output format.

General
-------

* Updated automatic Geant4 from AFS to version 10.3.p01 (latest available).
* Updated automatic ROOT from AFS to version 6.06.08.
* Remove support for ROOT v5 and require v6 onward.

Utilities
---------

* Python utilities now use a setup.py compatible with PIP.
* pybdsim v1.3
* pymadx v1.0
* pymad8 v0.8
* pytransport v0.1 (new)


V0.992 - 2017 / 07 / 26
=======================

New Features
------------

* Preparsing for GDML for pygeometry generated geometry - overcomes Geant4's GDML parser
  deficiency of only allowing one GDML file to be loaded in the whole program.
* Visualisation of multiple beam lines.
* Option to use first sampler emittance as assumed emittance throughout lattice in optical
  function calculation.
* Additional materials for LHC tunnel and geometry.

Bug Fixes
---------

* Fixed uncaught exception in analysis DataLoader class construction depending on optional arguments.
* BDSIM installation no longer dependent on build files - ROOT analysis dictionaries would use
  build headers. Issues #197 and #199.
* Fixed magnitude of B-field in rfcavity that resulted in extraordinarily strong B-fields.
* Fixed rf E- and pill-box fields ignoring phase and being a factor of 2:math:`\pi` too low in frequency.
* Fixed for crash when particle was at exactly 0 in a quadrupole.
* Fixed compiler warnings for Clang 8.1.
* Fixed all variable shadowing throughout codebase and reintroduced compiler warning if present.
* Fixed field transform for tilted and offset magnets. This fixes incorrect tracking for tilted
  magnets when using the "bdsimtwo" integrator set for dipoles.

General
-------

* Separated field and geometry in rf cavity element.
* Revised implementation of rf cavity construction for greater flexibility.
* RF cavity phase now automatically calculated based on location in lattice.
* Removal of old ROOT analysis scripts for very old data format.
* Revised construction to allow construction of multiple beam lines dynamically.

Utilities
---------

* pymadx v0.9
* pybdsim v1.2
* pymad8 v0.7


V0.991 - 2017 / 07 / 04
=======================

Bug Fixes
---------

* Fixed simple histogram weighting with Boolean expressions in rebdsim.
* Fixed comparator always failing.
* Fixed loading default event information in analysis.
* Fixed executing BDSIM from outside the directory containing the main gmad file.
* Fixed charge dependence of thin multipole and dipole fringe integrators.
* Scaled dipole fringe and thin multipole by momentum of particle.
* Fixed for loading geometry files prefixed with superfluous './'.
* Fixed for duplicate run beam on icon in visualiser for Geant4.10.3 onward.


General
-------

* Separated Doxygen cmake into own macro.

Utilities
---------

* pymadx v0.9
* pybdsim v1.1
* pymad8 v0.7


V0.99 - 2017 / 06 / 09
======================

New Features
------------

 * Parser will expand values from structures when printing. Issue #167.
 * Optical physics example and test. Issue #156.
 * Improved parser error messages. Issue #170.
 * Support for compressed input coordinate files for beam distributions using tar and gz.
 * Switch entirely to CMake labels instead of naming convention for tests.
 * AWAKE experiment code refactored into module.
 * New *shield* element that is a drift surrounded by rectangle of material.
 * New *placement* element that allows an object with geometry to be placed independent of the beam line.
 * maximumTrackLength option to limit any track in the geometry.
 * Ability to offset beam line w.r.t. world coordinates at start.
 * Check for required Geant4 environment variables.
 * Thin horizontal and vertical (and combined) kickers.
 * Thin multipoles.
 * Compatibility with Geant4.10.3.

Fields & Integrators
^^^^^^^^^^^^^^^^^^^^
 * Complete refactorisation of field classes and construction.
 * Centralised construction of fields.
 * 1-4D BDSIM format field map loading.
 * 2D Poisson SuperFish SF7 format field map loading.
 * 1-4D nearest neighbour, linear and cubic interpolators for field maps.
 * Support for compressed field maps using tar and gz.
 * Ability to choose integrator sets for all elements via parser.
 * Removal of all individual magnet classes - centralised construction in BDSMagnet.
 * New executable - "bdsinterpolator" - allows loaded and interpolated field to be queried and written out.
 * Rewritten dipole integrator using Geant4's helical stepper.
 * All integrators tested for low energy spiralling particles.
 * Introduction of visualisation commands.

Geometry
^^^^^^^^

 * Rewritten external geometry loading.
 * Ability to overlay externally provided geometry on magnets (except sbend).
 * Automatically generated tight-fitting containers for externally loaded GDML geometry.
 * *circularvacuum* beam pipe geometry that allows no geometry for the beam pipe; only vacuum.
 * Magnet colours tweaked slightly - pybdsim now matches BDSIM colour-wise.
 * Additional curvilinear bridge world to ensure continuous curvilinear coordinates.


Output & Analysis
^^^^^^^^^^^^^^^^^

 * Protection against invalid sampler names that would cause ROOT branching errors.
 * 1x 3D histogram in default output that can be placed along the beam line.
 * Support for 3D histograms in rebdsim.
 * All magnet strength components written out to survey.
 * Change of syntax in rebdsim analysis file to specify dimensions of histogram.
 * Stricter parsing of analysisConfig.txt for syntax checking.
 * New executable rebdsimOrbit to extract single orbit from sampler data.


Bug Fixes
---------

 * ASCII seed state can be loaded properly. Issue #163.
 * rfcavity can be created without a cavitymodel instance. Issue #165.
 * Memory leak in comparator event tree comparison fixed. Issue #169.
 * Zero angle bend with finite field can be created. Issue #176.
 * Samplers are compared properly in comparator. Issue #177.
 * Sampler names in Model tree now match those exactly in the Event tree.
 * Missing virtual keyword from destructors fixed through to fix leaks at the end of the program.
 * GFlash parameterisation is only loaded if specified in physics list.
 * Fixed geometry construction errors that may occur due to dynamic tolerances for physically large models.
 * Fixed infinite loop events if the primary vertex starts outside the world volume.
 * Regions and biases set correctly to components in BDSLine class.
 * Circle distribution did not have central value offsets.
 * Fix double registration of pion decay as well as some others for muons when using muon physics list.
 * Particles from physics list are now constructed correctly allowing more
   particles to be used in the beam definition.
 * Removal of Cherenkov radiation from muon physics significantly reducing simulation time.
 * Fix double registration of pion decay with muon physics list.
 * Issue #134 - samplers cause tracking warning.
 * Long running events due to spiralling particles. Issues #178, #132, #187.

General
-------

 * ``Sampler_`` prefix was removed from all samplers in rootevent output.
 * Sampler thickness reduced from 40 nm to 10 pm.
 * Removal of unnecessary step length limit to half the length of an element.
 * Revised region construction allowing arbitrary number to be constructed.
 * Revised bend construction with reduced volume count in some cases.

Utilities
---------

* pymadx v0.8
* pybdsim v1.0
* pymad8 v0.7

V0.95 - 2016 / 11 / 07
======================

New Features
------------

* Comparator program introduced for statistical comparison against reference results.
* rebdsim analysis examples and tests added.
* ROOT examples and tests added for analysis code usage in ROOT.
* Discrete optics only program (rebdsimOptics) added.
* Update CLHEP in AFS build to 2.3.1.0 for apple and 2.3.3.0 for RHL6.
* Reduced compilation time.

Bug Fixes
---------

* Fixed geometry tolerance issue that would cause Geant4 run-time errors for
  regular geometry for some particularly large spatial size models.
* Fixed for linker error with ZLIB and gzstream. Issues #9, #155, #158.
* Fixed NaN errors in certain circumstances when calculating optical functions.
* Fixed shadowing compilation warnings.
* Fixed geometry overlaps in rf cavity geometry. Issue #136.
* Coverity fixes for uninitialised variables / small memory leaks. Issues #152, #156.
* Fixed potential magnet geometry errors when creating very thin components.
* Fixed negative interaction warnings due to biasing. Issue #141.

General
-------

* Deprecate BDSIM Plank scattering, laserwire calorimeter and Bremsstrahlung
  lead particle biasing.

Utilities
---------
* pymadx v0.7
* pybdsim v0.9
* pymad8 v0.6
* robdsim v0.7

V0.94 - 2016 / 09 / 13
======================

New Features
------------

Analysis
^^^^^^^^

* Analysis class has been refactored to allow analysis on any tree in the BDSIM
  ROOT event output format using rebdsim.

Geometry
^^^^^^^^

* Tilted dipoles are now supported.
* The Read-Out geometry has been moved to the Geant4 parallel world scheme.
* The parallel sensitive geometry for tunnel hits has been deprecated and the
  functionality now provided by the new parallel read-out geometry.
* The read-out geometry construction has moved out of BDSAcceleratorComponent
  to its own geometry factory.
* Beam pipes are now constructed with arbitrary 3-vector surface normals, rather
  than angled faces described by an angle only in the x-z plane.
* The side of the yoke of a dipole with poled geometry can now be controlled with
  the :code:`yokeOnLeft` option.
* New interfaces to the auxiliary navigator have been written that use the mid-point
  of a step to much more robustly locate the required volume in the parallel
  curvilinear geometry.
* Overlap checking between adjacent dipoles with pole face rotations (and even tilts)
  prevents overlaps in geometry.

Output
^^^^^^

* Tunnel hits are now of the same type as general energy loss hits.
* The track ID can now be optionally written out to energy deposition hits.

Parser
^^^^^^

* The option :code:`modularPhysicsListsOn` has been deprecated.
* New per-element parameter :code:`yokeOnLeft`.

Physics
^^^^^^^

* The modular physics lists are now compulsory and the old physics construction has
  been deprecated.
* The existing BDSIM laserwire Compton scattering process construction was moved to
  the modular physics list scheme.


Bug Fixes
---------

Geometry
^^^^^^^^

* Extent inheritance with BDSGeoemetryComponent was fixed resolving rare overlaps.
* Poled geometry variable clean up fixed to ensure components from factories aren't
  related to each other - could cause rare crash on exit.
* Fixed extents and possible overlaps in tunnel 'rectaboveground' geometry.

Output
^^^^^^

* Energy deposition coordinates are randomly chosen along the step of the deposition. The
  x,y,z coordinates now match the random point, whereas they were the post step point previously.

Physics
^^^^^^^

* Fixed an issue that would cause infinite loops with strong process biasing.

General
^^^^^^^

* Initialisation of variables fixed throughout.
* Significantly improved compilation speed.
* Removed executable permission on all source files.

Utilities
---------
* pymadx v0.6
* pybdsim v0.8
* pymad8 v0.5
* robdsim v0.7


V0.93 - 2016 / 08 / 24
======================

New Features
------------

Analysis
^^^^^^^^

* New analysis tool 'rebdsim' replaces robdsim.
* Analysis directory with event, model, event info and options analysis.
* Histogram merging with correct statistical uncertainties.
* Deprecated root utilities to :code:`analysis/old/`.
* Rewritten optical function calculation with validated calculation.

Build
^^^^^

* Require CMake 2.8.12 or higher.
* Require Geant4 compiled with external CLHEP - ensures strong reproducibility.
* Start of bootstrapping scripts in :code:`depend/`.
* Factorisation of BDSIM's cmake package finding into :code:`cmake/`.


Geometry
^^^^^^^^

* Coil geometry introduced to generic library magnets.
* Overlap checking between magnets with pole face rotations.
* Collimator colour can now be controlled.
* End pieces for coils also introduced
* Default poled dipole geometry is now a C-shaped magnet with yoke on inside of bend.

Output
^^^^^^

* Switched to rootevent as default and **recommended** format.
* Include full set of options used in simulation in output.
* Include software version in output.
* Store seed state per event.
* Store histograms per event.
* Run and event durations stored in output.
* Output written in event of a crash.
* Refactor of trajectory information.
* Write out primary trajectory points.

Parser
^^^^^^

* Factorised options into optionsBase that is simple structure for easy saving.

Physics
^^^^^^^

* "Modular physics" list is now the default.
* Use geant4 helper class for physics lists construction to ensure correct order.
* Ability to provide a default bias to all types of volumes (vacuum, accelerator, all).
* Attribute energy deposition (uniformly) randomly along the step where it occurred for more accurate energy deposition - currently only s, not x,y,z - they represent before, after.
* 'solid' air materials for cross-section validation.
* Seed states are saved and restored in the primary generator action rather than event action.

Tracking
^^^^^^^^

* Geant4 Runge-Kutta stepper for quadrupole and sextupole for increased robustness.


General
^^^^^^^

* Strong recreation for an event by setting seed state issue (#118, #139).
* A BDSAcceleratorComponent can own an associated end piece(s) (before and after).
* A BDSAcceleratorComponent can have a input and output angled face.
* Halo bunch distribution developed significantly.
* Revised executable options for recreation / using a seed state.
* Signal handling improved.
* Templated user bunch file - can now use gzip compressed files.
* Improved default options for more realistic geometry.


Bug Fixes
---------

Geometry
^^^^^^^^

* Fixed loading for multiple GDML files having conflicting 'world' volumes.
* Reimplementation of pole geometry fixes gaps in poles (issue #110).

Parser
^^^^^^

* Fixed issue of parser python interface (issue #133).

Physics
^^^^^^^

* Modular physics lists are truly modular - fixes segfaults (issue #130).

Tracking
^^^^^^^^

* Fixes for cavity field values (issue #124).
* Fixed field value transform in sextupoles and above for global / local coordinates.
* Auxiliary navigator used more routinely for transforms with optional caching.

General
^^^^^^^

* Issues #115, #127, #129, #131


Utilities
---------
* pymadx v0.6
* pybdsim v0.7
* pymad8 v0.41
* robdsim v0.7


V0.92 - 2016 / 03 / 29
======================

New Features
------------

* Samplers are attached at the *exit* instead of the *entrance* of an element.
* Poleface rotations for bends are implemented (issue #100).
* Geant4 9.6.x versions support has been dropped (issue #111).
* DUMP element removed (issue #116).

Geometry
^^^^^^^^

* Samplers are no longer placed in the physical world but in a parallel sampler world.
* Above ground 'tunnel' geometry implemented.
* Introduced new RF cavity geometry and fields (still in development).

Output
^^^^^^

* ROOT version 6 support (issue #114).
* Option to fill ROOT with double or float precision.

Parser
^^^^^^

* Support for string variable (issue #126).

Physics
^^^^^^^

* Modular physics lists are default (issue #121).
* Use Geant4 provided synchrotron radiation instead of BDSIM one.

Bug fixes
---------

Geometry
^^^^^^^^

* Tunnel geometry fixes (issues #88 and #89).

Parser
^^^^^^

* Fixed fast list insertion (issue #113).
* Support for tildes in path names (issue #119).

Physics
^^^^^^^

* Old physics code cleanup (issue #123).
* Physics biasing properly initialised (issue #84).

General
^^^^^^^

* PDF Manual builds on Ubuntu (issue #85).
* 1D Histogram class significantly faster for uneven bin width histograms.

Utilities
---------
* pymadx v0.4
* pybdsim v0.5
* pymad8 v0.3
* robdsim v0.5

V0.91 - 2015 / 12 / 17
======================

New Features
------------

* New tests for file IO, coordinate transforms, aperture models, extra optical lattice patterns and general ring examples.

Geometry
^^^^^^^^

* Race track and octagonal aperture models introduced.
* New wedged energy degrader component introduced.

Output
^^^^^^

* Optional reduced number of variables in ROOT output - formats now "root" and "rootdetailed" (issue #107)
* Forced dependency on ROOT

Parser
^^^^^^

* Parser warns for redefined variable, and exits for usage of undeclared variables (issue #98)
* Parser reorganised to C++ class structure (issue #77)
* Command line options more flexible (issue #105)
* Ability to define Geant4 regions as objects in parser.
* Can attach samplers to all elements of one type (i.e. collimators).

Physics
^^^^^^^

* Can attach biasing to any part of any element from parser.

Tracking
^^^^^^^^

* Ability to start bunch from any S-position along accelerator, rather than just at beginning.

Bug fixes
---------

Geometry
^^^^^^^^

* Fixed for LHC detailed geometry when beam shield is rotated.
* Consolidation and improvement of aperture parameter validity testing.
* Fixed for femtometre occasional overlaps in magnet outer geometry.
* Fixed placement overlaps in rbend.
* Fixed segfault with RfCavity at end of run.
* Fixed crashes with zero angle sector bends.

Parser
^^^^^^

* Multiple command line arguments without space will now be recognised and highlighted.

Physics
^^^^^^^

* Made required version of Geant4 consistent across biasing code.

Tracking
^^^^^^^^

* Fields only constructed if non-zero strength used - avoids tracking errors for zero strength components.
* Fixed several issues with vertical and horizontal kicker construction and tracking.
* Broken external magnet fields disabled by default.
* Circular turn counting bugs fixed.
* Particles no longer killed with circular flag on if starting slightly behind starting midpoint.
* Particles no longer stepped by teleporter at beginning of 1st turn if starting behind starting midpoint.
* Fixed teleporter tracking for backwards travelling particles that would get stuck in a loop.

General
^^^^^^^

* Add CMake protection against Geant4 built with multithreading on (issue #103).

Utilities
---------
* pymadx v0.3
* pybdsim v0.4
* pymad8 v0.2
* robdsim v0.4

V0.9 - 2015 / 11 / 10
=====================

New Features
------------

* Physics biasing with ability to change cross-section for individual particles
  and processes, as well as attach to a variety of objects
* Decapole magnet
* Robdsim analysis package as separate executable for testing
* Tracking tester
* Improved C++11 use and iterator implementation across containers
* Can fill histogram with energy hit over a range covering several bins
* Introduced a separate auxiliary G4Navigator to avoid accidentally moving
  the particle during tracking when querying global-to-local transforms.
* Transform for curvilinear coordinates to global coordinates
  so primaries in those coordinates can be injected from anywhere (issue #63)
* Parser put in GMAD namespace
* New executable options for writing out geometry coordinates as built by BDSIM
* Magnets now have tightly fitting invisible container volumes, as opposed to
  large boxes before.
* Changed return type of magnet outer geometry factories to new BDSMagnetOuter
  class. This is because the container construction is now delegated to the
  magnet outer factory for tight-fitting container volumes.
* Extended examples and tests
* Move entirely to Geant4 visualisation manager supporting all available visualisers
  available with the local Geant4 installation

Bug fixes
---------

Geometry
^^^^^^^^

* Fixed bug where the read-out coordinates would also be offset by the offset
  of the element.
* Fixed overlaps in read out geometry.
* Reduced duplication in magnet outer factories.
* Fixed overlaps in rbend geometry (issue #64).
* Increased tolerance for sector bends (issue #73).
* Protected against zero angle sector bends (issue #74).
* Fixed overlaps in GDML geometry (issue #81).
* Geometry fixes (issues #76, 94, 95)

Physics
^^^^^^^

Parser
^^^^^^

* Occasional material parser segfault fixed (issue #25)
* Improved syntax checking and to not ignore unknown keywords (issue #71)
* Element extension fixed (issue #87)

Tracking
^^^^^^^^

* Dipole uses local coordinates and can bend in any direction (issue #78)

General
^^^^^^^

* Samplers can be attached to occurrence of a duplicated element (issue #47).
* Output survey updated and fixed (issue #60)
* Check for Geant4 environment variables (issue #62)
* Consistent policy for overwriting output files (issue #65)
* Improved memory and CPU for output writing (issue #86)

Utilities
---------
* pymadx v0.2
* pybdsim v0.3
* pymad8 v0.2
* robdsim v0.3

V0.8 - 2015 / 08 / 10
=====================

New Features
------------

* Tunnel geometry and flexible tunnel factories for different styles
* Tunnel read out geometry introduced for coordinates along tunnel axis
* C++11 adopted (required)
* `stopSecondaries` option
* Removed dependency on boost (issue #57)
* Restructured examples directory - top level contains only full machines
  and sub-directories contain features
* Example documentation in manual and in place beside each example with
  example screenshots
* Updated python utilities *pybdsim v0.1*, *pymadx v0.1*, *pymad8 v0.1* and *robdsim v0.2*
* Repeated components are not duplicated in memory - previously, they would
  be repeatedly constructed. Reduced memory footprint.
* Component information comes from Physical Volumes instead of Logical Volumes
* Improved manual documentation
* Improved Doxygen documentation
* Rubbish collection for all objects, rather than relying on only one run and
  Geant4 (partial) rubbish collection.
* String representation of enum types leading to more readable output
* Introduced ability to switch to new modular physics lists with flexible
  construction and addition of physics lists without hard-coded names for each
  combination - the user must turn this on explicitly

Bug fixes
---------

Geometry
^^^^^^^^
* Geometry overlaps (issues #55 and #58)
* Transform3d fix (issue #54)
* Fixed placement of objects outside x,z global plane - rotation bug, similarly
  for read-out geometry placement.
* Fixed broken circular control - bug was introduced in v0.7 - (issue #51).
* Strict checking of read out geometry construction to avoid invalid solids that
  would cause Geant4 to exit and BDSIM to crash.
* Strict checking on teleporter volume construction for circular machines that
  would cause Geant4 to exit and BDSIM to crash.
* Fixed calculation of length of sector bend magnet that would cause it to be
  slightly short - introduced in v0.7.
* Removed stored axes of rotation due to better implementation in BDSBeamline,
  avoiding duplication of information.
* Fixed issue of zero angle rbends causing a crash (issue #44).
* Event number print-out is now dynamic and based on the number of events to be
  generated. It is also controllable with the `printModuloFraction` option.
* Protected against bad user-specified values of `lengthSafety` to avoid
  geometry overlaps.
* Improved parser speed.

Physics
^^^^^^^
* SR radiation fixed in dipole (issue #53).
* Removed continuous synchrotron radiation, as it traps particles in low step-size
  infinite loop.
* Removal of poorly set deltaIntersection, chordStepMinimum and lengthSafety
  variables from examples - these should be left unset unless the user knows
  their purpose.

Output
^^^^^^
* Changed all transverse output units to **metres** - manual updated accordingly.
* Changed `z` in ASCII output to **global Z** instead of local z.
* Recorded energy in output is now unweighted, but energy recorded in convenient
  energy loss histogram is. Could have lead to double weighting previously.
* Fixed global coordinates being written out as local coordinates in ROOT
  output.
* Random number generator seed state not written out when no output is specified.

Parser
^^^^^^
* Return error if superfluous arguments are present (issue #56).
* Make parser more robust against duplicate element names (issue #43).
* Fixed warnings about compiling c as c++ being deprecated behaviour.

General
^^^^^^^
* Fixed wrong print out warning due to logic error (issue #51).
* Fixed boundary effects of energy deposition (issue #52).
* Fixed large memory leak for events with large number of particles - was due to
  accumulation of BDSTrajectory objects.


V0.702 2015 / 07 / 28 - Hotfix
==============================

* Fixes for physics production range cuts were not obeyed in simulation.

V0.701 2015 / 07 / 02 - Hotfix
==============================

* Fix for global X-coordinate not written to output for energy deposition.

V0.7 - 2015 / 06 / 30
=====================

New Features
------------

* Ability to write no output
* New magnet geometry factories introduced with seven possible magnet types.
* Introduction of --vis_debug flag to see container volumes without debug build.
* Revised magnet colours (same base colour, just prettier variant)
* New manual using sphinx documentation system
* Default visualiser provided - no requirement for a vis.mac by the user
* Nicer visualisation GUI by default
* Improved visualisation for GDML geometry
* Support for all Geant4 visualisers introduced (issue #11).

Bug fixes
---------

* Fixed overlapping volumes and tracking errors in beam pipes.
* Fixes for wrong transverse coordinates for geometry other than cylindrical magnets (issue #30).
* Histograms are now written to disk in case of crash or kill signal (issue #38).
* Fix for uncontrolled memory consumption for synchrotron radiation (issue #36).
* Fixed syntax error in parser on windows end of line character (issue #40).
* Follow user paths properly (issue #24).
* Parser can end on commented line (issue #41).
* Introduction of more flexible and weighted halo bunch distribution.
* Significant tidy of BDSAcceleratorComponent base class and derived classes.
* Fix LHC magnet geometry overlaps and improve efficiency as well as more flexible with different beam pipes.
* New BDSBeamline class used for component placement consistently in code.

V0.65 - 2015 / 04 / 10
======================

* New base class for any geometrical object BDSGeometryComponent
* New interchangeable beam pipes with six possible beam pipe shapes
* New sensitive detector manager to hold single instance of sd classes
* Introduction of G4Galactic material for 'empty' volumes rather than beam pipe vacuum
* Possibility to write to multiple output formats at once
* Extensive removal of unnecessary headers throughout
* Updated python utilities
* Fix for muon spoiler magnetic field (thanks to B. Pilicer)
* Fix for invisible cylinder of iron surrounding drifts previously


V0.64 - 2015 / 02 / 16
======================

* New histogram manager and factorisation of histograms from outputs
* Extra per-element histograms
* Basic implementation of valid solenoid

V0.63 - 2015 / 02 / 06
======================

* Large angle sbends split into multiple sbends, based on aperture error tolerance - currently 1mm.
* New geometry construction and placement for sbends and rbends - no overlapping volumes and
  simpler / increased performance
* Proper building under c++11 if available
* Introduction of composite bunch distribution
* Drop support for Geant4 versions 9.5 and older

V0.62 - 2014 / 08 / 07
======================

V0.61 - 2014 / 08 / 05
======================

* Geant4 version 10 support

v0.6 - 2013 / 12 / 02
=====================


v0.5 - 2008 / 11 / 08
=====================


v0.4 - 2008 / 02 / 26
=====================


v0.3 - 2007 / 01 / 26
=====================


v0.2 - 2006 / 05 / 18
=====================


v0.1 - 2006 / 02 / 22
=====================


beta - 2005 / 05 / 01
=====================
