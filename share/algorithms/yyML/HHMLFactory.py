import ROOT
import QFramework


def addAlgorithms(visitor, config):
    template_obj = ROOT.HHMLFactory

    test_list = template_obj(
        "3l0tau",
        "NOSYS",

        # Electron Map
        {
            'analysis_flag': ['el_isAnalysisElectron_NOSYS', 'char'],
            'pt': ['el_pt_NOSYS', 'float'],
            'eta': ['el_eta', 'float'],
            'phi': ['el_phi', 'float'],
            'charge': ['el_charge', 'float'],
            'IDLoose_ISOLoose': ['el_baselineSelection_LooseBLayerLH_Loose_VarRad_NOSYS', 'char'],
            'IDTight_ISOTight': ['el_baselineSelection_TightLH_Tight_VarRad_NOSYS', 'char'],
        },

        # Muon Map
        {
            'analysis_flag': ['mu_isAnalysisMuon_NOSYS', 'char'],
            'pt': ['mu_pt_NOSYS', 'float'],
            'eta': ['mu_eta', 'float'],
            'phi': ['mu_phi', 'float'],
            'charge': ['mu_charge', 'float'],
            'IDLoose_ISOLoose': ['mu_baselineSelection_Loose_PflowLoose_VarRad_NOSYS', 'char'],
            'IDTight_ISOTight': ['mu_baselineSelection_Medium_PflowTight_VarRad_NOSYS', 'char'],
        },

        # Tau Map
        {
            'analysis_flag': ['tau_isAnalysisTau_NOSYS', 'char'],
            'pt': ['tau_pt_NOSYS', 'float'],
            'eta': ['tau_eta', 'float'],
            'phi': ['tau_phi', 'float'],
            'charge': ['tau_charge', 'float'],
            'n_prong': ['tau_nProng', 'int'],
        },

        # Jet Map
        {
            'analysis_flag': ['recojet_antikt4PFlow_isAnalysisJet_NOSYS', 'char'],
            'pt': ['recojet_antikt4PFlow_pt_NOSYS', 'float'],
            'eta': ['recojet_antikt4PFlow_eta', 'float'],
            'phi': ['recojet_antikt4PFlow_phi', 'float'],
            'b_tag': ['recojet_antikt4PFlow_ftag_select_GN2v01_FixedCutBEff_85', 'char'],
        }
    )

    visitor.addAlgorithm(test_list)

    return True
