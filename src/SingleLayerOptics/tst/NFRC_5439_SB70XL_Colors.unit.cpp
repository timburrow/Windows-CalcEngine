#include <memory>
#include <gtest/gtest.h>

#include "WCESingleLayerOptics.hpp"
#include "WCESpectralAveraging.hpp"

using FenestrationCommon::CSeries;

class TestNFRC_5439_SB70XL_Colors_Scattering : public testing::Test
{
private:
    std::shared_ptr<SingleLayerOptics::ColorProperties<SingleLayerOptics::CScatteringLayer>>
      m_Color;

    std::vector<double> loadWavelengths() const
    {
        std::vector<double> aWavelengths{
          0.380, 0.385, 0.390, 0.395, 0.400, 0.405, 0.410, 0.415, 0.420, 0.425, 0.430, 0.435,
          0.440, 0.445, 0.450, 0.455, 0.460, 0.465, 0.470, 0.475, 0.480, 0.485, 0.490, 0.495,
          0.500, 0.505, 0.510, 0.515, 0.520, 0.525, 0.530, 0.535, 0.540, 0.545, 0.550, 0.555,
          0.560, 0.565, 0.570, 0.575, 0.580, 0.585, 0.590, 0.595, 0.600, 0.605, 0.610, 0.615,
          0.620, 0.625, 0.630, 0.635, 0.640, 0.645, 0.650, 0.655, 0.660, 0.665, 0.670, 0.675,
          0.680, 0.685, 0.690, 0.695, 0.700, 0.705, 0.710, 0.715, 0.720, 0.725, 0.730, 0.735,
          0.740, 0.745, 0.750, 0.755, 0.760, 0.765, 0.770, 0.775, 0.780};

        return aWavelengths;
    }

    std::shared_ptr<CSeries> loadSolarRadiationFile() const
    {
        // Full CIE Illuminant D651 nm ssp table (used for PHOTOPIC properties)
        auto solarRadiation = CSeries::create(
          {{0.300, 0.0341000}, {0.301, 0.3601400}, {0.302, 0.6861800}, {0.303, 1.0122200},
           {0.304, 1.3382600}, {0.305, 1.6643000}, {0.306, 1.9903400}, {0.307, 2.3163800},
           {0.308, 2.6424200}, {0.309, 2.9684600}, {0.310, 3.2945000}, {0.311, 4.9886500},
           {0.312, 6.6828000}, {0.313, 8.3769500}, {0.314, 10.071100}, {0.315, 11.765200},
           {0.316, 13.459400}, {0.317, 15.153500}, {0.318, 16.847700}, {0.319, 18.541800},
           {0.320, 20.236000}, {0.321, 21.917700}, {0.322, 23.599500}, {0.323, 25.281200},
           {0.324, 26.963000}, {0.325, 28.644700}, {0.326, 30.326500}, {0.327, 32.008200},
           {0.328, 33.690000}, {0.329, 35.371700}, {0.330, 37.053500}, {0.331, 37.343000},
           {0.332, 37.632600}, {0.333, 37.922100}, {0.334, 38.211600}, {0.335, 38.501100},
           {0.336, 38.790700}, {0.337, 39.080200}, {0.338, 39.369700}, {0.339, 39.659300},
           {0.340, 39.948800}, {0.341, 40.445100}, {0.342, 40.941400}, {0.343, 41.437700},
           {0.344, 41.934000}, {0.345, 42.430200}, {0.346, 42.926500}, {0.347, 43.422800},
           {0.348, 43.919100}, {0.349, 44.415400}, {0.350, 44.911700}, {0.351, 45.084400},
           {0.352, 45.257000}, {0.353, 45.429700}, {0.354, 45.602300}, {0.355, 45.775000},
           {0.356, 45.947700}, {0.357, 46.120300}, {0.358, 46.293000}, {0.359, 46.465600},
           {0.360, 46.638300}, {0.361, 47.183400}, {0.362, 47.728500}, {0.363, 48.273500},
           {0.364, 48.818600}, {0.365, 49.363700}, {0.366, 49.908800}, {0.367, 50.453900},
           {0.368, 50.998900}, {0.369, 51.544000}, {0.370, 52.089100}, {0.371, 51.877700},
           {0.372, 51.666400}, {0.373, 51.455000}, {0.374, 51.243700}, {0.375, 51.032300},
           {0.376, 50.820900}, {0.377, 50.609600}, {0.378, 50.398200}, {0.379, 50.186900},
           {0.380, 49.975500}, {0.381, 50.442800}, {0.382, 50.910000}, {0.383, 51.377300},
           {0.384, 51.844600}, {0.385, 52.311800}, {0.386, 52.779100}, {0.387, 53.246400},
           {0.388, 53.713700}, {0.389, 54.180900}, {0.390, 54.648200}, {0.391, 57.458900},
           {0.392, 60.269500}, {0.393, 63.080200}, {0.394, 65.890900}, {0.395, 68.701500},
           {0.396, 71.512200}, {0.397, 74.322900}, {0.398, 77.133600}, {0.399, 79.944200},
           {0.400, 82.754900}, {0.401, 83.628000}, {0.402, 84.501100}, {0.403, 85.374200},
           {0.404, 86.247300}, {0.405, 87.120400}, {0.406, 87.993600}, {0.407, 88.866700},
           {0.408, 89.739800}, {0.409, 90.612900}, {0.410, 91.486000}, {0.411, 91.680600},
           {0.412, 91.875200}, {0.413, 92.069700}, {0.414, 92.264300}, {0.415, 92.458900},
           {0.416, 92.653500}, {0.417, 92.848100}, {0.418, 93.042600}, {0.419, 93.237200},
           {0.420, 93.431800}, {0.421, 92.756800}, {0.422, 92.081900}, {0.423, 91.406900},
           {0.424, 90.732000}, {0.425, 90.057000}, {0.426, 89.382100}, {0.427, 88.707100},
           {0.428, 88.032200}, {0.429, 87.357200}, {0.430, 86.682300}, {0.431, 88.500600},
           {0.432, 90.318800}, {0.433, 92.137100}, {0.434, 93.955400}, {0.435, 95.773600},
           {0.436, 97.591900}, {0.437, 99.410200}, {0.438, 101.22800}, {0.439, 103.04700},
           {0.440, 104.86500}, {0.441, 106.07900}, {0.442, 107.29400}, {0.443, 108.50800},
           {0.444, 109.72200}, {0.445, 110.93600}, {0.446, 112.15100}, {0.447, 113.36500},
           {0.448, 114.57900}, {0.449, 115.79400}, {0.450, 117.00800}, {0.451, 117.08800},
           {0.452, 117.16900}, {0.453, 117.24900}, {0.454, 117.33000}, {0.455, 117.41000},
           {0.456, 117.49000}, {0.457, 117.57100}, {0.458, 117.65100}, {0.459, 117.73200},
           {0.460, 117.81200}, {0.461, 117.51700}, {0.462, 117.22200}, {0.463, 116.92700},
           {0.464, 116.63200}, {0.465, 116.33600}, {0.466, 116.04100}, {0.467, 115.74600},
           {0.468, 115.45100}, {0.469, 115.15600}, {0.470, 114.86100}, {0.471, 114.96700},
           {0.472, 115.07300}, {0.473, 115.18000}, {0.474, 115.28600}, {0.475, 115.39200},
           {0.476, 115.49800}, {0.477, 115.60400}, {0.478, 115.71100}, {0.479, 115.81700},
           {0.480, 115.92300}, {0.481, 115.21200}, {0.482, 114.50100}, {0.483, 113.78900},
           {0.484, 113.07800}, {0.485, 112.36700}, {0.486, 111.65600}, {0.487, 110.94500},
           {0.488, 110.23300}, {0.489, 109.52200}, {0.490, 108.81100}, {0.491, 108.86500},
           {0.492, 108.92000}, {0.493, 108.97400}, {0.494, 109.02800}, {0.495, 109.08200},
           {0.496, 109.13700}, {0.497, 109.19100}, {0.498, 109.24500}, {0.499, 109.30000},
           {0.500, 109.35400}, {0.501, 109.19900}, {0.502, 109.04400}, {0.503, 108.88800},
           {0.504, 108.73300}, {0.505, 108.57800}, {0.506, 108.42300}, {0.507, 108.26800},
           {0.508, 108.11200}, {0.509, 107.95700}, {0.510, 107.80200}, {0.511, 107.50100},
           {0.512, 107.20000}, {0.513, 106.89800}, {0.514, 106.59700}, {0.515, 106.29600},
           {0.516, 105.99500}, {0.517, 105.69400}, {0.518, 105.39200}, {0.519, 105.09100},
           {0.520, 104.79000}, {0.521, 105.08000}, {0.522, 105.37000}, {0.523, 105.66000},
           {0.524, 105.95000}, {0.525, 106.23900}, {0.526, 106.52900}, {0.527, 106.81900},
           {0.528, 107.10900}, {0.529, 107.39900}, {0.530, 107.68900}, {0.531, 107.36100},
           {0.532, 107.03200}, {0.533, 106.70400}, {0.534, 106.37500}, {0.535, 106.04700},
           {0.536, 105.71900}, {0.537, 105.39000}, {0.538, 105.06200}, {0.539, 104.73300},
           {0.540, 104.40500}, {0.541, 104.36900}, {0.542, 104.33300}, {0.543, 104.29700},
           {0.544, 104.26100}, {0.545, 104.22500}, {0.546, 104.19000}, {0.547, 104.15400},
           {0.548, 104.11800}, {0.549, 104.08200}, {0.550, 104.04600}, {0.551, 103.64100},
           {0.552, 103.23700}, {0.553, 102.83200}, {0.554, 102.42800}, {0.555, 102.02300},
           {0.556, 101.61800}, {0.557, 101.21400}, {0.558, 100.80900}, {0.559, 100.40500},
           {0.560, 100.00000}, {0.561, 99.633400}, {0.562, 99.266800}, {0.563, 98.900300},
           {0.564, 98.533700}, {0.565, 98.167100}, {0.566, 97.800500}, {0.567, 97.433900},
           {0.568, 97.067400}, {0.569, 96.700800}, {0.570, 96.334200}, {0.571, 96.279600},
           {0.572, 96.225000}, {0.573, 96.170300}, {0.574, 96.115700}, {0.575, 96.061100},
           {0.576, 96.006500}, {0.577, 95.951900}, {0.578, 95.897200}, {0.579, 95.842600},
           {0.580, 95.788000}, {0.581, 95.077800}, {0.582, 94.367500}, {0.583, 93.657300},
           {0.584, 92.947000}, {0.585, 92.236800}, {0.586, 91.526600}, {0.587, 90.816300},
           {0.588, 90.106100}, {0.589, 89.395800}, {0.590, 88.685600}, {0.591, 88.817700},
           {0.592, 88.949700}, {0.593, 89.081800}, {0.594, 89.213800}, {0.595, 89.345900},
           {0.596, 89.478000}, {0.597, 89.610000}, {0.598, 89.742100}, {0.599, 89.874100},
           {0.600, 90.006200}, {0.601, 89.965500}, {0.602, 89.924800}, {0.603, 89.884100},
           {0.604, 89.843400}, {0.605, 89.802600}, {0.606, 89.761900}, {0.607, 89.721200},
           {0.608, 89.680500}, {0.609, 89.639800}, {0.610, 89.599100}, {0.611, 89.409100},
           {0.612, 89.219000}, {0.613, 89.029000}, {0.614, 88.838900}, {0.615, 88.648900},
           {0.616, 88.458900}, {0.617, 88.268800}, {0.618, 88.078800}, {0.619, 87.888700},
           {0.620, 87.698700}, {0.621, 87.257700}, {0.622, 86.816700}, {0.623, 86.375700},
           {0.624, 85.934700}, {0.625, 85.493600}, {0.626, 85.052600}, {0.627, 84.611600},
           {0.628, 84.170600}, {0.629, 83.729600}, {0.630, 83.288600}, {0.631, 83.329700},
           {0.632, 83.370700}, {0.633, 83.411800}, {0.634, 83.452800}, {0.635, 83.493900},
           {0.636, 83.535000}, {0.637, 83.576000}, {0.638, 83.617100}, {0.639, 83.658100},
           {0.640, 83.699200}, {0.641, 83.332000}, {0.642, 82.964700}, {0.643, 82.597500},
           {0.644, 82.230200}, {0.645, 81.863000}, {0.646, 81.495800}, {0.647, 81.128500},
           {0.648, 80.761300}, {0.649, 80.394000}, {0.650, 80.026800}, {0.651, 80.045600},
           {0.652, 80.064400}, {0.653, 80.083100}, {0.654, 80.101900}, {0.655, 80.120700},
           {0.656, 80.139500}, {0.657, 80.158300}, {0.658, 80.177000}, {0.659, 80.195800},
           {0.660, 80.214600}, {0.661, 80.420900}, {0.662, 80.627200}, {0.663, 80.833600},
           {0.664, 81.039900}, {0.665, 81.246200}, {0.666, 81.452500}, {0.667, 81.658800},
           {0.668, 81.865200}, {0.669, 82.071500}, {0.670, 82.277800}, {0.671, 81.878400},
           {0.672, 81.479100}, {0.673, 81.079700}, {0.674, 80.680400}, {0.675, 80.281000},
           {0.676, 79.881600}, {0.677, 79.482300}, {0.678, 79.082900}, {0.679, 78.683600},
           {0.680, 78.284200}, {0.681, 77.427900}, {0.682, 76.571600}, {0.683, 75.715300},
           {0.684, 74.859000}, {0.685, 74.002700}, {0.686, 73.146500}, {0.687, 72.290200},
           {0.688, 71.433900}, {0.689, 70.577600}, {0.690, 69.721300}, {0.691, 69.910100},
           {0.692, 70.098900}, {0.693, 70.287600}, {0.694, 70.476400}, {0.695, 70.665200},
           {0.696, 70.854000}, {0.697, 71.042800}, {0.698, 71.231500}, {0.699, 71.420300},
           {0.700, 71.609100}, {0.701, 71.883100}, {0.702, 72.157100}, {0.703, 72.431100},
           {0.704, 72.705100}, {0.705, 72.979000}, {0.706, 73.253000}, {0.707, 73.527000},
           {0.708, 73.801000}, {0.709, 74.075000}, {0.710, 74.349000}, {0.711, 73.074500},
           {0.712, 71.800000}, {0.713, 70.525500}, {0.714, 69.251000}, {0.715, 67.976500},
           {0.716, 66.702000}, {0.717, 65.427500}, {0.718, 64.153000}, {0.719, 62.878500},
           {0.720, 61.604000}, {0.721, 62.432200}, {0.722, 63.260300}, {0.723, 64.088500},
           {0.724, 64.916600}, {0.725, 65.744800}, {0.726, 66.573000}, {0.727, 67.401100},
           {0.728, 68.229300}, {0.729, 69.057400}, {0.730, 69.885600}, {0.731, 70.405700},
           {0.732, 70.925900}, {0.733, 71.446000}, {0.734, 71.966200}, {0.735, 72.486300},
           {0.736, 73.006400}, {0.737, 73.526600}, {0.738, 74.046700}, {0.739, 74.566900},
           {0.740, 75.087000}, {0.741, 73.937600}, {0.742, 72.788100}, {0.743, 71.638700},
           {0.744, 70.489300}, {0.745, 69.339800}, {0.746, 68.190400}, {0.747, 67.041000},
           {0.748, 65.891600}, {0.749, 64.742100}, {0.750, 63.592700}, {0.751, 61.875200},
           {0.752, 60.157800}, {0.753, 58.440300}, {0.754, 56.722900}, {0.755, 55.005400},
           {0.756, 53.288000}, {0.757, 51.570500}, {0.758, 49.853100}, {0.759, 48.135600},
           {0.760, 46.418200}, {0.761, 48.456900}, {0.762, 50.495600}, {0.763, 52.534400},
           {0.764, 54.573100}, {0.765, 56.611800}, {0.766, 58.650500}, {0.767, 60.689200},
           {0.768, 62.728000}, {0.769, 64.766700}, {0.770, 66.805400}, {0.771, 66.463100},
           {0.772, 66.120900}, {0.773, 65.778600}, {0.774, 65.436400}, {0.775, 65.094100},
           {0.776, 64.751800}, {0.777, 64.409600}, {0.778, 64.067300}, {0.779, 63.725100},
           {0.780, 63.382800}, {0.781, 63.474900}, {0.782, 63.567000}, {0.783, 63.659200},
           {0.784, 63.751300}, {0.785, 63.843400}, {0.786, 63.935500}, {0.787, 64.027600},
           {0.788, 64.119800}, {0.789, 64.211900}, {0.790, 64.304000}, {0.791, 63.818800},
           {0.792, 63.333600}, {0.793, 62.848400}, {0.794, 62.363200}, {0.795, 61.877900},
           {0.796, 61.392700}, {0.797, 60.907500}, {0.798, 60.422300}, {0.799, 59.937100},
           {0.800, 59.451900}, {0.801, 58.702600}, {0.802, 57.953300}, {0.803, 57.204000},
           {0.804, 56.454700}, {0.805, 55.705400}, {0.806, 54.956200}, {0.807, 54.206900},
           {0.808, 53.457600}, {0.809, 52.708300}, {0.810, 51.959000}, {0.811, 52.507200},
           {0.812, 53.055300}, {0.813, 53.603500}, {0.814, 54.151600}, {0.815, 54.699800},
           {0.816, 55.248000}, {0.817, 55.796100}, {0.818, 56.344300}, {0.819, 56.892400},
           {0.820, 57.440600}, {0.821, 57.727800}, {0.822, 58.015000}, {0.823, 58.302200},
           {0.824, 58.589400}, {0.825, 58.876500}, {0.826, 59.163700}, {0.827, 59.450900},
           {0.828, 59.738100}, {0.829, 60.025300}, {0.830, 60.312500}});

        return solarRadiation;
    }

    std::shared_ptr<SpectralAveraging::CSpectralSampleData> loadSampleData_NFRC_5439() const
    {
        return SpectralAveraging::CSpectralSampleData::create(
          {{0.300, 0.0019, 0.0491, 0.2686},  {0.305, 0.0037, 0.0885, 0.2723},
           {0.310, 0.0062, 0.1118, 0.2856},  {0.315, 0.0114, 0.1363, 0.3101},
           {0.320, 0.0209, 0.1595, 0.3418},  {0.325, 0.0289, 0.1802, 0.3774},
           {0.330, 0.0404, 0.1970, 0.4123},  {0.335, 0.0605, 0.2098, 0.4415},
           {0.340, 0.0653, 0.2192, 0.4636},  {0.345, 0.0590, 0.2253, 0.4796},
           {0.350, 0.0559, 0.2295, 0.4911},  {0.355, 0.0583, 0.2325, 0.4986},
           {0.360, 0.0657, 0.2341, 0.5029},  {0.365, 0.0780, 0.2351, 0.5018},
           {0.370, 0.0959, 0.2374, 0.4958},  {0.375, 0.1206, 0.2435, 0.4837},
           {0.380, 0.1534, 0.2485, 0.4627},  {0.385, 0.1954, 0.2512, 0.4324},
           {0.390, 0.2466, 0.2485, 0.3929},  {0.395, 0.3066, 0.2391, 0.3465},
           {0.400, 0.3718, 0.2233, 0.2960},  {0.410, 0.5010, 0.1781, 0.1981},
           {0.420, 0.5990, 0.1340, 0.1288},  {0.430, 0.6539, 0.1040, 0.0927},
           {0.440, 0.6761, 0.0893, 0.0782},  {0.450, 0.6823, 0.0841, 0.0732},
           {0.460, 0.6858, 0.0833, 0.0710},  {0.470, 0.6893, 0.0840, 0.0694},
           {0.480, 0.6948, 0.0850, 0.0677},  {0.490, 0.7028, 0.0858, 0.0663},
           {0.500, 0.7102, 0.0859, 0.0652},  {0.510, 0.7179, 0.0856, 0.0648},
           {0.520, 0.7232, 0.0850, 0.0647},  {0.530, 0.7272, 0.0840, 0.0648},
           {0.540, 0.7296, 0.0829, 0.0650},  {0.550, 0.7311, 0.0811, 0.0642},
           {0.560, 0.7315, 0.0785, 0.0624},  {0.570, 0.7315, 0.0744, 0.0588},
           {0.580, 0.7293, 0.0686, 0.0529},  {0.590, 0.7270, 0.0618, 0.0456},
           {0.600, 0.7224, 0.0545, 0.0377},  {0.610, 0.7155, 0.0483, 0.0308},
           {0.620, 0.7025, 0.0456, 0.0280},  {0.630, 0.6810, 0.0492, 0.0324},
           {0.640, 0.6507, 0.0629, 0.0487},  {0.650, 0.6096, 0.0886, 0.0787},
           {0.660, 0.5585, 0.1279, 0.1245},  {0.670, 0.5003, 0.1795, 0.1843},
           {0.680, 0.4374, 0.2398, 0.2544},  {0.690, 0.3754, 0.3047, 0.3291},
           {0.700, 0.3171, 0.3709, 0.4036},  {0.710, 0.2651, 0.4348, 0.4750},
           {0.720, 0.2194, 0.4917, 0.5401},  {0.730, 0.1807, 0.5413, 0.5955},
           {0.740, 0.1494, 0.5855, 0.6448},  {0.750, 0.1233, 0.6244, 0.6877},
           {0.760, 0.1025, 0.6563, 0.7243},  {0.770, 0.0852, 0.6826, 0.7536},
           {0.780, 0.0716, 0.7092, 0.7786},  {0.790, 0.0602, 0.7228, 0.8010},
           {0.800, 0.0509, 0.7419, 0.8199},  {0.810, 0.0436, 0.7579, 0.8362},
           {0.820, 0.0373, 0.7693, 0.8507},  {0.830, 0.0320, 0.7820, 0.8617},
           {0.840, 0.0280, 0.7918, 0.8711},  {0.850, 0.0234, 0.7952, 0.8792},
           {0.860, 0.0206, 0.8083, 0.8928},  {0.870, 0.0184, 0.8075, 0.8935},
           {0.880, 0.0161, 0.8138, 0.9007},  {0.890, 0.0151, 0.8183, 0.9063},
           {0.900, 0.0131, 0.8220, 0.9101},  {0.910, 0.0117, 0.8248, 0.9148},
           {0.920, 0.0108, 0.8258, 0.9189},  {0.930, 0.0099, 0.8283, 0.9225},
           {0.940, 0.0089, 0.8305, 0.9256},  {0.950, 0.0080, 0.8321, 0.9283},
           {0.960, 0.0075, 0.8337, 0.9310},  {0.970, 0.0067, 0.8355, 0.9338},
           {0.980, 0.0061, 0.8373, 0.9360},  {0.990, 0.0058, 0.8391, 0.9380},
           {1.000, 0.0053, 0.8410, 0.9400},  {1.050, 0.0036, 0.8478, 0.9475},
           {1.100, 0.0014, 0.8533, 0.9534},  {1.150, 0.0010, 0.8604, 0.9575},
           {1.200, 0.0005, 0.8639, 0.9608},  {1.250, 0.0003, 0.8699, 0.9634},
           {1.300, 0.0002, 0.8740, 0.9655},  {1.350, 0.0000, 0.8747, 0.9672},
           {1.400, 0.0005, 0.8759, 0.9682},  {1.450, 0.0004, 0.8835, 0.9692},
           {1.500, 0.0003, 0.8897, 0.9702},  {1.550, 0.0003, 0.8951, 0.9713},
           {1.600, 0.0003, 0.8984, 0.9719},  {1.650, 0.0003, 0.8999, 0.9727},
           {1.700, 0.0006, 0.9005, 0.9734},  {1.750, 0.0007, 0.8974, 0.9739},
           {1.800, 0.0005, 0.8887, 0.9745},  {1.850, 0.0005, 0.8806, 0.9751},
           {1.900, 0.0004, 0.8788, 0.9754},  {1.950, 0.0004, 0.8757, 0.9756},
           {2.000, 0.0005, 0.8688, 0.9759},  {2.050, 0.0005, 0.8626, 0.9764},
           {2.100, 0.0005, 0.8565, 0.9766},  {2.150, 0.0003, 0.8407, 0.9767},
           {2.200, 0.0008, 0.7981, 0.9768},  {2.250, 0.0001, 0.7954, 0.9771},
           {2.300, 0.0008, 0.7861, 0.9763},  {2.350, 0.0010, 0.7911, 0.9768},
           {2.400, 0.0004, 0.7827, 0.9781},  {2.450, 0.0013, 0.7314, 0.9773},
           {2.500, 0.0009, 0.7243, 0.9779},  {5.000, 0.0000, 0.0346, 0.9854},
           {6.000, 0.0000, 0.0278, 0.9847},  {7.000, 0.0000, 0.0168, 0.9850},
           {8.000, 0.0000, 0.0011, 0.9857},  {9.000, 0.0000, 0.1913, 0.9868},
           {10.000, 0.0000, 0.2714, 0.9845}, {11.000, 0.0000, 0.1950, 0.9841},
           {12.000, 0.0000, 0.0881, 0.9846}, {13.000, 0.0000, 0.0775, 0.9848},
           {14.000, 0.0000, 0.0715, 0.9850}, {15.000, 0.0000, 0.0603, 0.9852},
           {16.000, 0.0000, 0.0495, 0.9855}, {17.000, 0.0000, 0.0392, 0.9863},
           {18.000, 0.0000, 0.0260, 0.9874}, {19.000, 0.0000, 0.0537, 0.9883},
           {20.000, 0.0000, 0.1614, 0.9885}, {21.000, 0.0000, 0.2107, 0.9872},
           {22.000, 0.0000, 0.2206, 0.9873}, {23.000, 0.0000, 0.2047, 0.9868},
           {24.000, 0.0000, 0.1862, 0.9864}, {25.000, 0.0000, 0.1683, 0.9870},
           {26.000, 0.0000, 0.1562, 0.9873}, {27.000, 0.0000, 0.1475, 0.9868},
           {28.000, 0.0000, 0.1405, 0.9880}, {29.000, 0.0000, 0.1357, 0.9858},
           {30.000, 0.0000, 0.1315, 0.9864}, {31.000, 0.0000, 0.1283, 0.9890},
           {32.000, 0.0000, 0.1278, 0.9870}, {33.000, 0.0000, 0.1284, 0.9872},
           {34.000, 0.0000, 0.1276, 0.9871}, {35.000, 0.0000, 0.1280, 0.9905},
           {36.000, 0.0000, 0.1304, 0.9899}, {37.000, 0.0000, 0.1331, 0.9901},
           {38.000, 0.0000, 0.1336, 0.9882}, {39.000, 0.0000, 0.1355, 0.9917},
           {40.000, 0.0000, 0.1381, 0.9915}});
    }

    std::shared_ptr<CSeries> ASTM_E308_1964_X() const
    {
        return CSeries::create(
          {{0.380, 0.0002}, {0.385, 0.0007}, {0.390, 0.0024}, {0.395, 0.0072}, {0.400, 0.0191},
           {0.405, 0.0434}, {0.410, 0.0847}, {0.415, 0.1406}, {0.420, 0.2045}, {0.425, 0.2647},
           {0.430, 0.3147}, {0.435, 0.3577}, {0.440, 0.3837}, {0.445, 0.3867}, {0.450, 0.3707},
           {0.455, 0.3430}, {0.460, 0.3023}, {0.465, 0.2541}, {0.470, 0.1956}, {0.475, 0.1323},
           {0.480, 0.0805}, {0.485, 0.0411}, {0.490, 0.0162}, {0.495, 0.0051}, {0.500, 0.0038},
           {0.505, 0.0154}, {0.510, 0.0375}, {0.515, 0.0714}, {0.520, 0.1177}, {0.525, 0.1730},
           {0.530, 0.2365}, {0.535, 0.3042}, {0.540, 0.3768}, {0.545, 0.4516}, {0.550, 0.5298},
           {0.555, 0.6161}, {0.560, 0.7052}, {0.565, 0.7938}, {0.570, 0.8787}, {0.575, 0.9512},
           {0.580, 1.0142}, {0.585, 1.0743}, {0.590, 1.1185}, {0.595, 1.1343}, {0.600, 1.1240},
           {0.605, 1.0891}, {0.610, 1.0305}, {0.615, 0.9507}, {0.620, 0.8563}, {0.625, 0.7549},
           {0.630, 0.6475}, {0.635, 0.5351}, {0.640, 0.4316}, {0.645, 0.3437}, {0.650, 0.2683},
           {0.655, 0.2043}, {0.660, 0.1526}, {0.665, 0.1122}, {0.670, 0.0813}, {0.675, 0.0529},
           {0.680, 0.0409}, {0.685, 0.0286}, {0.690, 0.0199}, {0.695, 0.0138}, {0.700, 0.0096},
           {0.705, 0.0066}, {0.710, 0.0046}, {0.715, 0.0031}, {0.720, 0.0022}, {0.725, 0.0015},
           {0.730, 0.0010}, {0.735, 0.0007}, {0.740, 0.0005}, {0.745, 0.0004}, {0.750, 0.0003},
           {0.755, 0.0002}, {0.760, 0.0001}, {0.765, 0.0001}, {0.770, 0.0001}, {0.775, 0.0000},
           {0.780, 0.0000}});
    }

    std::shared_ptr<CSeries> ASTM_E308_1964_Y() const
    {
        return CSeries::create(
          {{0.380, 0.0000}, {0.385, 0.0001}, {0.390, 0.0003}, {0.395, 0.0008}, {0.400, 0.0020},
           {0.405, 0.0045}, {0.410, 0.0088}, {0.415, 0.0145}, {0.420, 0.0214}, {0.425, 0.0295},
           {0.430, 0.0387}, {0.435, 0.0496}, {0.440, 0.0621}, {0.445, 0.0747}, {0.450, 0.0895},
           {0.455, 0.1063}, {0.460, 0.1282}, {0.465, 0.1528}, {0.470, 0.1852}, {0.475, 0.2199},
           {0.480, 0.2536}, {0.485, 0.2977}, {0.490, 0.3391}, {0.495, 0.3954}, {0.500, 0.4608},
           {0.505, 0.5314}, {0.510, 0.6067}, {0.515, 0.6857}, {0.520, 0.7618}, {0.525, 0.8233},
           {0.530, 0.8752}, {0.535, 0.9238}, {0.540, 0.9620}, {0.545, 0.9822}, {0.550, 0.9918},
           {0.555, 0.9991}, {0.560, 0.9973}, {0.565, 0.9824}, {0.570, 0.9555}, {0.575, 0.9152},
           {0.580, 0.8689}, {0.585, 0.8256}, {0.590, 0.7774}, {0.595, 0.7204}, {0.600, 0.6583},
           {0.605, 0.5939}, {0.610, 0.5280}, {0.615, 0.4618}, {0.620, 0.3981}, {0.625, 0.3396},
           {0.630, 0.2835}, {0.635, 0.2283}, {0.640, 0.1798}, {0.645, 0.1402}, {0.650, 0.1076},
           {0.655, 0.0812}, {0.660, 0.0603}, {0.665, 0.0441}, {0.670, 0.0318}, {0.675, 0.0226},
           {0.680, 0.0159}, {0.685, 0.0111}, {0.690, 0.0077}, {0.695, 0.0054}, {0.700, 0.0037},
           {0.705, 0.0026}, {0.710, 0.0018}, {0.715, 0.0012}, {0.720, 0.0008}, {0.725, 0.0006},
           {0.730, 0.0004}, {0.735, 0.0003}, {0.740, 0.0002}, {0.745, 0.0001}, {0.750, 0.0001},
           {0.755, 0.0001}, {0.760, 0.0000}, {0.765, 0.0000}, {0.770, 0.0000}, {0.775, 0.0000},
           {0.780, 0.0000}});
    }


    std::shared_ptr<CSeries> ASTM_E308_1964_Z() const
    {
        return CSeries::create(
          {{0.380, 0.0007}, {0.385, 0.0029}, {0.390, 0.0105}, {0.395, 0.0323}, {0.400, 0.0860},
           {0.405, 0.1971}, {0.410, 0.3894}, {0.415, 0.6568}, {0.420, 0.9725}, {0.425, 1.2825},
           {0.430, 1.5535}, {0.435, 1.7985}, {0.440, 1.9673}, {0.445, 2.0273}, {0.450, 1.9948},
           {0.455, 1.9007}, {0.460, 1.7454}, {0.465, 1.5549}, {0.470, 1.3176}, {0.475, 1.0302},
           {0.480, 0.7721}, {0.485, 0.5701}, {0.490, 0.4153}, {0.495, 0.3024}, {0.500, 0.2185},
           {0.505, 0.1592}, {0.510, 0.1120}, {0.515, 0.0822}, {0.520, 0.0607}, {0.525, 0.0431},
           {0.530, 0.0305}, {0.535, 0.0206}, {0.540, 0.0137}, {0.545, 0.0079}, {0.550, 0.0040},
           {0.555, 0.0011}, {0.560, 0.0000}, {0.565, 0.0000}, {0.570, 0.0000}, {0.575, 0.0000},
           {0.580, 0.0000}, {0.585, 0.0000}, {0.590, 0.0000}, {0.595, 0.0000}, {0.600, 0.0000},
           {0.605, 0.0000}, {0.610, 0.0000}, {0.615, 0.0000}, {0.620, 0.0000}, {0.625, 0.0000},
           {0.630, 0.0000}, {0.635, 0.0000}, {0.640, 0.0000}, {0.645, 0.0000}, {0.650, 0.0000},
           {0.655, 0.0000}, {0.660, 0.0000}, {0.665, 0.0000}, {0.670, 0.0000}, {0.675, 0.0000},
           {0.680, 0.0000}, {0.685, 0.0000}, {0.690, 0.0000}, {0.695, 0.0000}, {0.700, 0.0000},
           {0.705, 0.0000}, {0.710, 0.0000}, {0.715, 0.0000}, {0.720, 0.0000}, {0.725, 0.0000},
           {0.730, 0.0000}, {0.735, 0.0000}, {0.740, 0.0000}, {0.745, 0.0000}, {0.750, 0.0000},
           {0.755, 0.0000}, {0.760, 0.0000}, {0.765, 0.0000}, {0.770, 0.0000}, {0.775, 0.0000},
           {0.780, 0.0000}});
    }

    std::unique_ptr<SingleLayerOptics::CScatteringLayer>
      createLayer(const std::shared_ptr<CSeries> & astmStandard) const
    {
        double thickness = 3.048e-3;   // [m]
        const auto aMaterial =
          SingleLayerOptics::Material::nBandMaterial(loadSampleData_NFRC_5439(),
                                                     astmStandard,
                                                     thickness,
                                                     FenestrationCommon::MaterialType::Monolithic,
                                                     FenestrationCommon::WavelengthRange::Visible);

        auto layer = wce::make_unique<SingleLayerOptics::CScatteringLayer>(
          SingleLayerOptics::CScatteringLayer::createSpecularLayer(aMaterial));
        layer->setSourceData(loadSolarRadiationFile());
        layer->setWavelengths(loadWavelengths());

        return layer;
    }

protected:
    void SetUp() override
    {
        auto LayerX = createLayer(ASTM_E308_1964_X());
        auto LayerY = createLayer(ASTM_E308_1964_Y());
        auto LayerZ = createLayer(ASTM_E308_1964_Z());

        CSeries DX = *ASTM_E308_1964_X();
        CSeries DY = *ASTM_E308_1964_Y();
        CSeries DZ = *ASTM_E308_1964_Z();

        CSeries solarRadiation = *loadSolarRadiationFile();

        auto wl = loadWavelengths();

        m_Color =
          std::make_shared<SingleLayerOptics::ColorProperties<SingleLayerOptics::CScatteringLayer>>(
            *LayerX, *LayerY, *LayerZ, solarRadiation, DX, DY, DZ, wl);
    }

public:
    std::shared_ptr<SingleLayerOptics::ColorProperties<SingleLayerOptics::CScatteringLayer>>
      getLayer() const
    {
        return m_Color;
    }
};

TEST_F(TestNFRC_5439_SB70XL_Colors_Scattering, TestTrichromatic_T)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    auto aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    SingleLayerOptics::Trichromatic T = aLayer->getTrichromatic(
      FenestrationCommon::PropertySimple::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(66.393144, T.X, 1e-6);
    EXPECT_NEAR(71.662457, T.Y, 1e-6);
    EXPECT_NEAR(71.768345, T.Z, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_Scattering, TestTrichromatic_R)
{
    SCOPED_TRACE("Begin Test: Trichromatic.");

    auto aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    SingleLayerOptics::Trichromatic T = aLayer->getTrichromatic(
      FenestrationCommon::PropertySimple::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(6.971494, T.X, 1e-6);
    EXPECT_NEAR(7.635557, T.Y, 1e-6);
    EXPECT_NEAR(10.159147, T.Z, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_Scattering, TestRGB_T)
{
    SCOPED_TRACE("Begin Test: RGB.");

    auto aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto rgb = aLayer->getRGB(
      FenestrationCommon::PropertySimple::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(239, rgb.R);
    EXPECT_EQ(245, rgb.G);
    EXPECT_EQ(233, rgb.B);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_Scattering, TestRGB_R)
{
    SCOPED_TRACE("Begin Test: RGB.");

    auto aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto rgb = aLayer->getRGB(
      FenestrationCommon::PropertySimple::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_EQ(75, rgb.R);
    EXPECT_EQ(88, rgb.G);
    EXPECT_EQ(96, rgb.B);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_Scattering, TestCIE_LAB_T)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    auto aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto cie = aLayer->getCIE_Lab(
      FenestrationCommon::PropertySimple::T, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(87.805864, cie.L, 1e-6);
    EXPECT_NEAR(-3.402796, cie.a, 1e-6);
    EXPECT_NEAR(4.081155, cie.b, 1e-6);
}

TEST_F(TestNFRC_5439_SB70XL_Colors_Scattering, TestCIE_LAB_R)
{
    SCOPED_TRACE("Begin Test: CIE_LAB.");

    auto aLayer = getLayer();

    FenestrationCommon::Side aSide = FenestrationCommon::Side::Front;

    auto cie = aLayer->getCIE_Lab(
      FenestrationCommon::PropertySimple::R, aSide, FenestrationCommon::Scattering::DirectDirect);
    EXPECT_NEAR(33.212062, cie.L, 1e-6);
    EXPECT_NEAR(-2.636088, cie.a, 1e-6);
    EXPECT_NEAR(-6.300395, cie.b, 1e-6);
}
