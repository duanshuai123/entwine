/******************************************************************************
* Copyright (c) 2019, Connor Manning (connor@hobu.co)
*
* Entwine -- Point cloud indexing
*
* Entwine is available under the terms of the LGPL2 license. See COPYING
* for specific license text and more information.
*
******************************************************************************/

#include "info.hpp"

#include <entwine/types/srs.hpp>
#include <entwine/types/manifest.hpp>
#include <entwine/util/fs.hpp>
#include <entwine/util/info.hpp>

namespace entwine
{
namespace app
{

void Info::addArgs()
{
    m_ap.setUsage("entwine info <path(s)> (<options>)");

    m_ap.add(
            "--manifest",
            "-m",
            "Create a buildable manifest.",
            [this](json j) { m_json["manifest"] = true; });

    addInput(
            "File paths or directory entries.  For a recursive directory "
            "search, the notation is 'directory/**'\n"
            "Example: --input path.laz, --input data-directory/",
            true);

    addOutput(
            "If provided, detailed per-file information will be written "
            "to this directory in JSON format\n"
            "Example: --output my-output/");

    addConfig();
    addTmp();
    addReprojection();
    addSimpleThreads();
    addNoTrustHeaders();
    addArbiter();
}

void Info::run()
{
    if (m_json.value("manifest", false))
    {
        /*
        StringList filenames(resolve(m_json.at("input").get<StringList>()));

        // Each filename may point at:
        //      A JSON file of detailed information.
        //      A point cloud file that needs to be analyzed.
        //      TODO: A path/points/bounds reference from a shallow scan.
        */
        const auto manifest(manifest::create(analyze(m_json)));

        return;
    }

    const auto sources = analyze(m_json);
    const auto reduced = source::reduce(sources);

    if (m_json.count("output"))
    {
        const std::string output(m_json.at("output").get<std::string>());
        const arbiter::Arbiter a(m_json.value("arbiter", json()).dump());

        if (a.isLocal(output)) arbiter::mkdirp(output);
        serialize(sources, a.getEndpoint(output));
    }

    std::cout << "Sources: " << json(sources).dump(2) << std::endl;
    std::cout << "Info: " << json(reduced).dump(2) << std::endl;
}

} // namespace app
} // namespace entwine