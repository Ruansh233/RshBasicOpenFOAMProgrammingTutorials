#include "customLibrary.H"

scalar computeR(const fvMesh& mesh, volScalarField& r, dimensionedVector x0)
{
    r = mag(mesh.C()-x0);
    // returnReduce will give the value on all processor
    return returnReduce(max(r).value(), maxOp<scalar>());
}

void computeU(const fvMesh& mesh, volVectorField& U, word pName)
{
    // This allows a reference to a field to be obtained through the mesh object
    // alone by just knowing the name of the field.
    const volScalarField& pField = mesh.lookupObject<volScalarField>(pName);

    // Do the usual
    U = fvc::grad(pField)*dimensionedScalar("tmp", dimTime, 1.);
}
