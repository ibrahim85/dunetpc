
simple_plugin(ClusterCounter "module"
			larcorealg_Geometry
			larcore_Geometry_Geometry_service
			${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
 			art_Persistency_Common canvas
			art_Persistency_Provenance canvas
			art_Utilities canvas
			${MF_MESSAGELOGGER}
			
			cetlib cetlib_except
			${ROOT_BASIC_LIB_LIST}
			BASENAME_ONLY
)

simple_plugin(ClusterCounter2 "module"
			lardataobj_RecoBase
			larcorealg_Geometry
			larcore_Geometry_Geometry_service
			${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
 			art_Persistency_Common canvas
			art_Persistency_Provenance canvas
			art_Utilities canvas
			${MF_MESSAGELOGGER}
			
			cetlib cetlib_except
			${ROOT_BASIC_LIB_LIST}
			BASENAME_ONLY
)

simple_plugin(ClusterCounter3 "module"
			lardataobj_RecoBase
			larcorealg_Geometry
			larcore_Geometry_Geometry_service
			${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
 			art_Persistency_Common canvas
			art_Persistency_Provenance canvas
			art_Utilities canvas
			${MF_MESSAGELOGGER}
			
			cetlib cetlib_except
			${ROOT_BASIC_LIB_LIST}
			BASENAME_ONLY
)

simple_plugin(ClusterCounter4 "module"
			lardataobj_RecoBase
			larcorealg_Geometry
			larcore_Geometry_Geometry_service
			larsim_MCCheater_BackTrackerService_service
                        larsim_MCCheater_ParticleInventoryService_service
			nusimdata_SimulationBase
			${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
 			art_Persistency_Common canvas
			art_Persistency_Provenance canvas
			art_Utilities canvas
			${MF_MESSAGELOGGER}
			
			cetlib cetlib_except
			${ROOT_BASIC_LIB_LIST}
			BASENAME_ONLY
)

simple_plugin(ClusterCounter5 "module"
			lardataobj_RecoBase
			larcorealg_Geometry
			larcore_Geometry_Geometry_service
			larsim_MCCheater_BackTrackerService_service
                        larsim_MCCheater_ParticleInventoryService_service
			nusimdata_SimulationBase
			${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
 			art_Persistency_Common canvas
			art_Persistency_Provenance canvas
			art_Utilities canvas
			${MF_MESSAGELOGGER}
			
			cetlib cetlib_except
			${ROOT_BASIC_LIB_LIST}
			BASENAME_ONLY
)

simple_plugin(ReadSpacePointAndCnn "module"
			lardata_ArtDataHelper
			lardataobj_RecoBase
			larcorealg_Geometry
			larcore_Geometry_Geometry_service
			nusimdata_SimulationBase
			${ART_FRAMEWORK_CORE}
			${ART_FRAMEWORK_PRINCIPAL}
			${ART_FRAMEWORK_SERVICES_REGISTRY}
			${ART_FRAMEWORK_SERVICES_OPTIONAL}
			${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
 			art_Persistency_Common canvas
			art_Persistency_Provenance canvas
			art_Utilities canvas
			${MF_MESSAGELOGGER}
			
			cetlib cetlib_except
			${ROOT_BASIC_LIB_LIST}
			BASENAME_ONLY
)

simple_plugin(ReadBeamInfo "module"
      nusimdata_SimulationBase
      larcorealg_Geometry
      ${ART_FRAMEWORK_CORE}
      ${ART_FRAMEWORK_PRINCIPAL}
      ${ART_FRAMEWORK_SERVICES_REGISTRY}
      ${ART_FRAMEWORK_SERVICES_OPTIONAL}
      ${ART_FRAMEWORK_SERVICES_OPTIONAL_TFILESERVICE_SERVICE}
      art_Persistency_Common canvas
      art_Persistency_Provenance canvas
      art_Utilities canvas
      ${MF_MESSAGELOGGER}
      
      cetlib cetlib_except
      ${ROOT_BASIC_LIB_LIST}
      BASENAME_ONLY
)

install_headers()
install_fhicl()
install_source()
install_scripts()
