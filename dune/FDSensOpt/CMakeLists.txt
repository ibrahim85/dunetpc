add_subdirectory(IniSegAlg)

simple_plugin(NueAna   "module"
		        RawData
		        RecoBase
		        RecoAlg
			AnalysisAlg
		        PMAlg
		        IniSegAlg
		        Geometry
			Geometry_service
			Utilities
			Simulation
			DetectorProperties_service
			LArProperties_service
			BackTracker_service
			${SIMULATIONBASE}
                        ${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL}
                        ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
 			${ART_PERSISTENCY_COMMON}
			${ART_PERSISTENCY_PROVENANCE}
			${ART_UTILITIES}
			${MF_MESSAGELOGGER}
			${MF_UTILITIES}
			${CETLIB}
			${ROOT_BASIC_LIB_LIST}
              BASENAME_ONLY
)

simple_plugin(IniSegReco "module"
		      RawData
		      RecoBase
		      RecoAlg
		      PMAlg
		      IniSegAlg
		      Geometry
		      Geometry_service
	              BackTracker_service
		      Utilities
		      Simulation
		      DetectorProperties_service
		      LArProperties_service
		      ${SIMULATIONBASE}
      		      ${ART_FRAMEWORK_CORE}
		      ${ART_FRAMEWORK_PRINCIPAL}
		      ${ART_FRAMEWORK_SERVICES_REGISTRY}
      		      ${ART_FRAMEWORK_SERVICES_OPTIONAL}
      		      ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
 		      ${ART_PERSISTENCY_COMMON}
		      ${ART_PERSISTENCY_PROVENANCE}
		      ${ART_UTILITIES}
		      ${MF_MESSAGELOGGER}
		      ${MF_UTILITIES}
		      ${CETLIB}
		      ${ROOT_BASIC_LIB_LIST}
              BASENAME_ONLY
)


install_headers()
install_fhicl()
install_source()
install_scripts()