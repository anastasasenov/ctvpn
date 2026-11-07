<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="text" encoding="UTF-8"/>

    <xsl:template match="/">
        <xsl:text>#pragma once&#10;</xsl:text>
        <xsl:text>#include "ctvpn.h"&#10;&#10;</xsl:text>
        <xsl:text>namespace generated_net {&#10;&#10;</xsl:text>

        <!-- Generate Place ID Constants -->
        <xsl:text>// --- Places ---&#10;</xsl:text>
        <xsl:for-each select="//*[local-name()='place']">
            <xsl:text>constexpr size_t P_</xsl:text>
            <xsl:value-of select="@id"/>
            <xsl:text> = </xsl:text>
            <xsl:value-of select="position() - 1"/>
            <xsl:text>;&#10;</xsl:text>
        </xsl:for-each>
        <xsl:text>&#10;</xsl:text>

        <!-- Generate Transitions -->
        <xsl:text>// --- Transitions ---&#10;</xsl:text>
        <xsl:for-each select="//*[local-name()='transition']">
            <xsl:variable name="trans_id" select="@id"/>
            <xsl:text>using T_</xsl:text>
            <xsl:value-of select="$trans_id"/>
            <xsl:text> = ctvpn::Transition&lt;&#10;</xsl:text>
            
            <!-- Input Arcs -->
            <xsl:text>    ctvpn::In&lt;</xsl:text>
            <xsl:for-each select="//*[local-name()='arc' and @target = $trans_id]">
                <xsl:variable name="source_place" select="@source"/>
                <xsl:text>ctvpn::Arc&lt;P_</xsl:text>
                <xsl:value-of select="$source_place"/>
                <xsl:text>, </xsl:text>
                <xsl:choose>
                    <xsl:when test="*[local-name()='inscription']/*[local-name()='text']">
                        <xsl:value-of select="*[local-name()='inscription']/*[local-name()='text']"/>
                    </xsl:when>
                    <xsl:otherwise>1</xsl:otherwise>
                </xsl:choose>
                <xsl:text>&gt;</xsl:text>
                <xsl:if test="position() != last()">, </xsl:if>
            </xsl:for-each>
            <xsl:text>&gt;,&#10;</xsl:text>

            <!-- Output Arcs -->
            <xsl:text>    ctvpn::Out&lt;</xsl:text>
            <xsl:for-each select="//*[local-name()='arc' and @source = $trans_id]">
                <xsl:variable name="target_place" select="@target"/>
                <xsl:text>ctvpn::Arc&lt;P_</xsl:text>
                <xsl:value-of select="$target_place"/>
                <xsl:text>, </xsl:text>
                <xsl:choose>
                    <xsl:when test="*[local-name()='inscription']/*[local-name()='text']">
                        <xsl:value-of select="*[local-name()='inscription']/*[local-name()='text']"/>
                    </xsl:when>
                    <xsl:otherwise>1</xsl:otherwise>
                </xsl:choose>
                <xsl:text>&gt;</xsl:text>
                <xsl:if test="position() != last()">, </xsl:if>
            </xsl:for-each>
            <xsl:text>&gt;&#10;</xsl:text>

            <xsl:text>&gt;;&#10;&#10;</xsl:text>
        </xsl:for-each>

        <xsl:text>} // namespace generated_net&#10;</xsl:text>
    </xsl:template>
</xsl:stylesheet>
