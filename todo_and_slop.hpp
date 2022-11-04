//
// Todo
//
{
    // Go through and __unbind__ things when done in functions, such as glBindTexture(0), glBindBuffer(0), glBind*, glEnable*
}

// Sphere
{
    constexpr float  ring_count            = 25.0f;
    constexpr float  points_per_ring_count = 55.0f;
    constexpr size_t sphere_index_count = static_cast<size_t>(ring_count * points_per_ring_count *
                                                              3);

    size_t          index   = 0;
    float           theta   = 0.0f;
    float           phi     = 0.0f;
    constexpr float d_theta = PI / ring_count;                // Spacing between rings.
    constexpr float d_phi   = 2 * PI / points_per_ring_count; // Spacing between points in a ring.

    float sphere_vertex_positions[sphere_index_count] = {};
    for (auto& ring_number = 0; ring_number < ring_count; ring_number++)
    {
        theta += d_theta;
        for (auto& point_number = 0; point_number < points_per_ring_count; point_number++)
        {
            phi += d_phi;
            sphere_vertex_positions[index++] = static_cast<float>(sin(theta) * cos(phi)); // X
            sphere_vertex_positions[index++] = static_cast<float>(sin(theta) * sin(phi)); // Y
            sphere_vertex_positions[index++] = static_cast<float>(cos(theta));            // Z
        }
    }

    sphere.ArrayBuffer(vao,
                       sphere_vertex_positions,
                       sizeof(sphere_vertex_positions),
                       (sphere_index_count / 3),
                       GL_STATIC_DRAW,
                       GL_TRIANGLE_FAN);
    sphere.FormatVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}