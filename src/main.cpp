#include <libff/common/default_types/ec_pp.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include <libsnark/gadgetlib1/protoboard.hpp>

using namespace libsnark;

/**
 * The code below provides an example of all stages of running a R1CS gppzkSNARK.
 *
 * Of course, in a real-life scenario, we would have three distinct entities,
 * mangled into one in the demonstration below. The three entities are as follows.
 * (1) The "generator", which runs the ppzkSNARK generator on input a given
 *     constraint system CS to create a proving and a verification key for CS.
 * (2) The "prover", which runs the ppzkSNARK prover on input the proving key,
 *     a primary input for CS, and an auxiliary input for CS.
 * (3) The "verifier", which runs the ppzkSNARK verifier on input the verification key,
 *     a primary input for CS, and a proof.
 */
template<typename ppT>
bool run_r1cs_ppzksnark(const r1cs_example<libff::Fr<ppT> > &example)
{
    libff::print_header("R1CS gppzkSNARK Generator");
    r1cs_ppzksnark_keypair<ppT> keypair = r1cs_ppzksnark_generator<ppT>(example.constraint_system);
    printf("\n"); libff::print_indent(); libff::print_mem("after generator");

    libff::print_header("Preprocess verification key");
    r1cs_ppzksnark_processed_verification_key<ppT> pvk = r1cs_ppzksnark_verifier_process_vk<ppT>(keypair.vk);

    libff::print_header("R1CS gppzkSNARK fake Prover");
    std::vector<libff::Fr<ppT>> primary_input;
    primary_input.resize(example.primary_input.size());
    for (size_t i = 0; i < example.primary_input.size(); i++) {
      primary_input[i] = libff::Fr<ppT>(2);
    }
    std::vector<libff::Fr<ppT>> auxiliary_input;
    auxiliary_input.resize(example.auxiliary_input.size());
    for (size_t i = 0; i < example.auxiliary_input.size(); i++) {
      auxiliary_input[i] = libff::Fr<ppT>(2);
    }
    r1cs_ppzksnark_proof<ppT> proof = r1cs_ppzksnark_prover<ppT>(keypair.pk, example.primary_input, example.auxiliary_input);
    printf("\n"); libff::print_indent(); libff::print_mem("after fake prover");

    libff::print_header("R1CS gppzkSNARK Verifier");
    const bool ans = r1cs_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, example.primary_input, proof);
    printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
    printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

    libff::print_header("R1CS gppzkSNARK Online Verifier");
    const bool ans2 = r1cs_ppzksnark_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof);
    assert(ans == ans2);

    return ans;
}

template<typename ppT>
void test_r1cs_ppzksnark(size_t num_constraints, size_t input_size)
{
    r1cs_example<libff::Fr<ppT> > example = generate_r1cs_example_with_binary_input<libff::Fr<ppT> >(num_constraints, input_size);
    const bool bit = run_r1cs_ppzksnark<ppT>(example);
    assert(bit);

}

int main () {
    default_r1cs_ppzksnark_pp::init_public_params();
    test_r1cs_ppzksnark<default_r1cs_ppzksnark_pp>(1000, 1000);

    return 0;
}
