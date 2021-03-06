#include <fbxppch.h>
#include <fbxpstate.h>
#include <queue>

void ExportMesh( FbxNode* node, fbxp::Node& n );
void ExportMaterials( FbxScene* scene );
void ExportMaterials( FbxNode* node, fbxp::Node& n );
void ExportTransform( FbxNode* node, fbxp::Node& n );
void ExportAnimation( FbxNode* node, fbxp::Node& n );

void ExportNodeAttributes( FbxNode* node, fbxp::Node& n ) {
    auto& s = fbxp::Get( );

    n.cullingType = (fbxp::fb::ECullingType) node->mCullingType;
    s.console->info( "Node \"{}\" has {} culling type.", node->GetName(), n.cullingType );

    ExportTransform( node, n );
    ExportAnimation( node, n );
    ExportMesh( node, n );
    ExportMaterials( node, n );
}

uint32_t ExportNode( FbxNode* node ) {
    auto& s = fbxp::Get( );

    const uint32_t nodeId = static_cast< uint32_t >( s.nodes.size( ) );
    s.nodes.emplace_back( );

    auto& n = s.nodes.back( );
    n.id = nodeId;
    n.nameId = s.PushName( node->GetName( ) );

    ExportNodeAttributes( node, n );
    if ( auto c = node->GetChildCount( ) ) {
        n.childIds.reserve( c );
        for ( auto i = 0; i < c; ++i ) {
            const auto childId = ExportNode( node->GetChild( i ) );
            s.nodes[ nodeId ].childIds.push_back( childId );
        }
    }

    return nodeId;
}

void ExportScene( FbxScene* scene ) {
    auto& s = fbxp::Get( );

    // Pre-allocate nodes and attributes.
    s.nodes.reserve( (size_t) scene->GetNodeCount( ) );
    s.meshes.reserve( (size_t) scene->GetNodeCount( ) );

    // We want shared materials, so export all the scene material first
    // and reference them from the node scope by their indices.
    ExportMaterials( scene );

    // Export nodes recursively.
    ExportNode( scene->GetRootNode( ) );
}
