<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">

  <table cellpadding="0" cellspacing="0">
    <tr class="audioGroup">
		<td id="tLine0" class="highlighted">
			<div class="jp"><strong><em>Benshi Narration (Musei)</em></strong></div>
			<div class="en"><strong>The Cabinet of Dr. Caligari</strong></div>
		</td>
	</tr>
    <xsl:for-each select="popcorn/timeline/transcriptText/lineGroup">
    <tr>	
      	<td>
			<xsl:attribute name="id">tLine<xsl:value-of select="@id" /></xsl:attribute>
			<xsl:attribute name="class">transcriptLineGroup</xsl:attribute>
			<xsl:variable name="tId"><xsl:value-of select="@id" /></xsl:variable>
			
			<xsl:variable name="timecode"><xsl:value-of select="/popcorn/timeline/transcripts/transcript[@tId=$tId]/@in" /></xsl:variable>
			
			<xsl:variable name="timeHrs" select="substring($timecode, 1, 2)" />
			<xsl:variable name="timeMin" select="substring($timecode, 4, 2)" />
			<xsl:variable name="timeSec" select="substring($timecode, 7, 2)" />
			<xsl:attribute name="data-time-in-ms"><xsl:value-of select="($timeHrs * 3600) + ($timeMin * 60) + ($timeSec)" /></xsl:attribute>
			
			<xsl:attribute name="data-time"><xsl:value-of select="/popcorn/timeline/transcripts/transcript[@tId=$tId]/@in" /></xsl:attribute>
		<xsl:for-each select="line">
			<div>
				<xsl:attribute name="class">
					<xsl:value-of select="@lang" />
				</xsl:attribute>
				<xsl:value-of select="." />
			</div>
		</xsl:for-each>
		</td>
    </tr>
    </xsl:for-each>
  </table>

</xsl:template>

</xsl:stylesheet>