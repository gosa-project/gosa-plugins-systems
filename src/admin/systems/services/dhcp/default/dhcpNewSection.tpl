<div class="dhcp-section-wrapper">
    <div class="row">
        <div class="col s12 xl12">
            <h3>{t}Create new DHCP section{/t}</h3>

            <p>
                <b>{t}Please choose one of the following DHCP section types.{/t}</b>
            </p>

            <div class="input-field">
                <select size="1" id="section" name="section" title="{t}Choose section type to create{/t}">
                    {html_options options=$sections}
                </select>
                <label for="section">{t}Section{/t}</label>
            </div>
        </div>
    </div>
</div>

<div class="card-action">
    <button class="btn-small primary" type='submit' name='create_section'>{t}Create{/t}</button>
    <button class="btn-small primary" type='submit' name='cancel_section'>
        {msgPool type=cancelButton}</button>
</div>

<!-- Place cursor -->
<script language="JavaScript" type="text/javascript">
    focus_field('section');
</script>