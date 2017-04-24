import os
import networkx as nx
import subprocess
from Bio import PDB


class ChainSplitter:
    def __init__(self, out_dir=None):
        """ Create parsing and writing objects, specify output directory. """
        self.parser = PDB.PDBParser()
        self.writer = PDB.PDBIO()
        if out_dir is None:
            out_dir = os.path.join(os.getcwd(), "chain_PDBs")
        self.out_dir = out_dir

    def make_pdb(self, pdb_path, chain_letters, overwrite=False, struct=None):
        """ Create a new PDB file containing only the specified chains.

        Returns the path to the created file.

        :param pdb_path: full path to the crystal structure
        :param chain_letters: iterable of chain characters (case insensitive)
        :param overwrite: write over the output file if it exists
        """
        chain_letters = [chain.upper() for chain in chain_letters]

        # Input/output files
        (pdb_dir, pdb_fn) = os.path.split(pdb_path)
        pdb_id = pdb_fn[0:4]
        out_name = "%s_%s.pdb" % (pdb_id, "".join(chain_letters))
        out_dir = os.path.join(self.out_dir, "".join(chain_letters))
        out_path = os.path.join(out_dir,out_name)

        if not os.path.exists(out_dir):
            os.makedirs(out_dir)

        print ("OUT PATH:",out_path)
        plural = "s" if (len(chain_letters) > 1) else ""  # for printing

        # Skip PDB generation if the file already exists
        if (not overwrite) and (os.path.isfile(out_path)):
            print("Chain%s %s of '%s' already extracted to '%s'." %
                  (plural, ", ".join(chain_letters), pdb_id, out_name))
            return out_path

        print("Extracting chain%s %s from %s..." % (plural,
                                                    ", ".join(chain_letters), pdb_fn))

        # Get structure, write new file with only given chains
        if struct is None:
            struct = self.parser.get_structure(pdb_id, pdb_path)
        self.writer.set_structure(struct)
        self.writer.save(out_path, select=SelectChains(chain_letters))

        return out_path


class SelectChains(PDB.Select):
    """ Only accept the specified chains when saving. """
    def __init__(self, chain_letters):
        self.chain_letters = chain_letters

    def accept_chain(self, chain):
        return (chain.get_id() in self.chain_letters)


if __name__ == "__main__":
    """ Parses PDB id's desired chains, and creates new PDB structures. """
    import sys
    import ntpath
    import shutil
    if not len(sys.argv) == 8:
        print ("Usage: $ python %s 'pdbfile.pdb' 'outputFolder' 'adp_em_exe' 'map_file' 'cutoff' "
               "'resolution' 'bw' "
               "" % __file__)
        sys.exit()

    pdb_textfn = sys.argv[1]

    parser = PDB.PDBParser()

    srcPDB = parser.get_structure(ntpath.basename(sys.argv[1]), sys.argv[1])
    (pdb_dir, pdb_fn) = os.path.split(sys.argv[1])

    out_path = os.path.join(sys.argv[2],pdb_fn[0:4])

    if not os.path.exists(out_path):
        os.makedirs(out_path)



    splitter = ChainSplitter(out_path)
    models = []
    for model in srcPDB:
        for chain in model:

            models.append(splitter.make_pdb(sys.argv[1], chain.get_id()))


    adp_em_exe = sys.argv[3]
    map = sys.argv[4]
    cutoff = sys.argv[5]
    res = sys.argv[6]
    bw = sys.argv[7]



    for model in models:
        (folder, asd ) = os.path.split(model)
        new_exe = shutil.copy(adp_em_exe, folder)
        print(folder)
        print(new_exe)
        process = subprocess.Popen([new_exe, map, model, bw, cutoff, res, "-n 5"], cwd=folder)
        process.wait()



    graph = nx.Graph()


    for model in models:
        (path, fileName) = os.path.split(model)
        for file in os.listdir(path):
            if file.startswith("adpEM"):
                newNode = fileName[:-4]+file[5:9]
                graph.add_node(newNode, file=file)
                for trans_node in graph.nodes():
                    pass ## score "file" node with trans_node[file]
