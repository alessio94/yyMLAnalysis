import ROOT
import QFramework

def addAlgorithms(visitor, config):
    template_obj = ROOT.HHMLFactory

    test_list = template_obj()

    # test_list = template_obj(
    #     "yyML",
    #     "NOSYS",

    #     # Photon Map

    #     # Electron Map
    #     {
    #         'analysis_flag': ['el_isAnalysisElectron_NOSYS', 'char'], # IDMediumLH_ISOLoose
    #         'pt': ['el_pt_NOSYS', 'float'],
    #         'eta': ['el_eta_NOSYS', 'float'],
    #         'phi': ['el_phi_NOSYS', 'float'],
    #         'charge': ['el_charge_NOSYS', 'float'],
    #         'IDMediumDNN_ISOLoose': ['el_baselineSelection_MediumDNN_Loose_VarRad_NOSYS', 'char'],
    #     },

    #     # Muon Map
    #     {
    #         'analysis_flag': ['mu_isAnalysisMuon_NOSYS', 'char'], # IDMedium_ISOLoose
    #         'pt': ['mu_pt_NOSYS', 'float'],
    #         'eta': ['mu_eta_NOSYS', 'float'],
    #         'phi': ['mu_phi_NOSYS', 'float'],
    #         'charge': ['mu_charge_NOSYS', 'float'],
    #     },

    #     # Tau Map
    #     {
    #         'analysis_flag': ['tau_isAnalysisTau_NOSYS', 'char'], # RNNLoose
    #         'pt': ['tau_pt_NOSYS', 'float'],
    #         'eta': ['tau_eta_NOSYS', 'float'],
    #         'phi': ['tau_phi_NOSYS', 'float'],
    #         'charge': ['tau_charge_NOSYS', 'float'],
    #         'n_prong': ['tau_nProng_NOSYS', 'int'],
    #         'RNNLoose_eleid': ['tau_baselineSelection_RNNLoose_eleid_NOSYS', 'char'], # RNNLoose with eVeto
    #     },

    #     # Jet Map
    #     {
    #         'analysis_flag': ['recojet_antikt4PFlow_jvt_selection_NOSYS', 'char'], # This just always passes, where is recojet_antikt4PFlow_isAnalysisJet_NOSYS?
    #         'pt': ['recojet_antikt4PFlow_pt_NOSYS', 'float'],
    #         'eta': ['recojet_antikt4PFlow_eta_NOSYS', 'float'],
    #         'phi': ['recojet_antikt4PFlow_phi_NOSYS', 'float'],
    #         'b_tag': ['recojet_antikt4PFlow_ftag_select_GN2v01_FixedCutBEff_77_NOSYS', 'char'],
    #     }
    # )

    visitor.addAlgorithm(test_list)

    return True
