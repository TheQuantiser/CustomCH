from skbuild import setup

# Packages providing configuration and data inputs
input_packages = {
    "CombineHarvester.CombineTools.input": "CombineTools/input",
    "CombineHarvester.CombineTools.input.examples": "CombineTools/input/examples",
    "CombineHarvester.CombineTools.input.job_prefixes": "CombineTools/input/job_prefixes",
    "CombineHarvester.CombineTools.input.xsecs_brs": "CombineTools/input/xsecs_brs",
}

package_data = {
    "CombineHarvester.CombineTools": ["*.so"],
    "CombineHarvester.CombineTools.scripts": ["*"],
    "CombineHarvester.CombinePdfs": ["*.so"],
}
for pkg in input_packages:
    package_data[pkg] = ["*"]

setup(
    name="combineharvester",
    version="0.1.0",
    description="CMS CombineHarvester tools",
    packages=[
        "CombineHarvester",
        "CombineHarvester.CombineTools",
        "CombineHarvester.CombineTools.combine",
        "CombineHarvester.CombineTools.systematics",
        "CombineHarvester.CombineTools.scripts",
        *input_packages,
        "CombineHarvester.CombinePdfs",
    ],
    package_dir={
        "CombineHarvester": "CombineHarvester",
        "CombineHarvester.CombineTools": "CombineTools/python",
        "CombineHarvester.CombineTools.combine": "CombineTools/python/combine",
        "CombineHarvester.CombineTools.systematics": "CombineTools/python/systematics",
        "CombineHarvester.CombineTools.scripts": "CombineTools/scripts",
        "CombineHarvester.CombinePdfs": "CombinePdfs/python",
        **input_packages,
    },
    package_data=package_data,
    include_package_data=True,
)
