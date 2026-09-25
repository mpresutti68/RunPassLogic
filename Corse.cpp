#include <vector>
#include <algorithm> 

int CCS(int ci, int cl, int ca,
    const std::vector<int>& cm, const std::vector<int>& cu) {
    int mm = -1;
    if (!cm.empty()) {
        mm = *std::max_element(cm.begin(), cm.end());
    }

    int mu = -1;
    if (!cu.empty()) {
        mu = *std::max_element(cu.begin(), cu.end());
    }

    int p = std::max(ci, ca + 1);
    int ls = std::min(cl, mu + 1);
    for (int cc = p; cc <= ls; ++cc) {
        bool im = (std::find(cm.begin(), cm.end(), cc) != cm.end());

        
        if (im) continue;

        return cc;
    }

    if (mm != -1) return mm;
    return ci;
}

extern "C" {
    __declspec(dllexport) int CCSI(
        int ci,
        int cl,
        int ca,
        const int* cmArr, int cmSize,
        const int* cuArr, int cuSize)
    {
        
        std::vector<int> m(cmArr, cmArr + cmSize);
        std::vector<int> u(cuArr, cuArr + cuSize);

        return CCS(ci, cl, ca, m, u);
    }
}

int TPNM(
    const std::vector<int>& cm, const std::vector<int>& cu) {

    int pct = -1; 

    for (int c_u : cu) {

        bool i_m = (std::find(cm.begin(), cm.end(), c_u) != cm.end());

       
        if (!i_m) {
            if (pct == -1 || c_u < pct) {
                pct = c_u;
            }
        }
    }

    return pct;
}
extern "C" {
    __declspec(dllexport) int TPNMI(
        const int* cmArr, int cmSize,
        const int* cuArr, int cuSize)
    {

        std::vector<int> m(cmArr, cmArr + cmSize);
        std::vector<int> u(cuArr, cuArr + cuSize);

        return TPNM(m, u);
    }
}